const net = require("net");
const tls = require('tls');
const fs = require('fs');
const util = require('util');

module.exports.createProxy = function(proxyPort,
    serviceHost, servicePort, options) {
    return new TcpProxy(proxyPort, serviceHost, servicePort, options);
};

function uniqueKey(socket) {
    var key = socket.remoteAddress + ":" + socket.remotePort;
    return key;
}

function parse(o) {
    if (typeof o === "string") {
        return o.split(",");
    } else if (typeof o === "number") {
        return parse(o.toString());
    } else if (Array.isArray(o)) {
        return o;
    } else {
        throw new Error("cannot parse object: " + o);
    }
}

function TcpProxy(proxyPort, serviceHost, servicePort, options) {
    this.proxyPort = proxyPort;
    this.serviceHosts = parse(serviceHost);
    this.servicePorts = parse(servicePort);
    this.serviceHostIndex = -1;
    this.options = this.parseOptions(options);
    
    // 暂时不管 TLS 的配置
    this.proxyTlsOptions = {
        passphrase: this.options.passphrase,
        secureProtocol: "TLSv1_2_method"
    };
    if (this.options.tls) {
        // eslint-disable-next-line security/detect-non-literal-fs-filename
        this.proxyTlsOptions.pfx = fs.readFileSync(this.options.pfx);
    }
    this.serviceTlsOptions = {
        rejectUnauthorized: this.options.rejectUnauthorized,
        secureProtocol: "TLSv1_2_method"
    };

    // 与客户端的连接通道存储
    this.proxySockets = {};
    if (this.options.identUsers.length !== 0) {
        this.users = this.options.identUsers;
        this.log('Only allow these users: '.concat(this.users.join(', ')));
    } else {
        this.log('Allow all users');
    }
    this.createListener();
}

TcpProxy.prototype.parseOptions = function(options) {
    return Object.assign({
        quiet: false,
        pfx: require.resolve('./cert.pfx'),
        passphrase: 'abcd',
        rejectUnauthorized: true,
        identUsers: []
    }, options);
};

TcpProxy.prototype.createListener = function() {
    var self = this;
    if (self.options.tls) {
        self.server = tls.createServer(self.proxyTlsOptions, function(socket) {
            self.handleClientConnection(socket);
        });
    } else {
        self.server = net.createServer(function(socket) {
            self.handleClientConnection(socket);
        });
    }
    self.server.listen(self.proxyPort, self.options.hostname);
};

TcpProxy.prototype.handleClientConnection = function(socket) {
    var self = this;
    if (self.users) {
        self.handleAuth(socket);
    } else {
        self.handleClient(socket);
    }
};

// RFC 1413 authentication
TcpProxy.prototype.handleAuth = function(proxySocket) {
    var self = this;
    var query = util.format("%d, %d", proxySocket.remotePort, this.proxyPort);
    var ident = new net.Socket();
    var resp = undefined;
    ident.on('error', function(e) {
        resp = false;
        ident.destroy();
    });
    ident.on('data', function(data) {
        resp = data.toString().trim();
        ident.destroy();
    });
    ident.on('close', function(data) {
        if (!resp) {
            self.log('No identd');
            proxySocket.destroy();
            return;
        }
        var user = resp.split(':').pop();
        if (!self.users.includes(user)) {
            self.log(util.format('User "%s" unauthorized', user));
            proxySocket.destroy();
        } else {
            self.handleClient(proxySocket);
        }
    });
    ident.connect(113, proxySocket.remoteAddress, function() {
        ident.write(query);
        ident.end();
    });
};

TcpProxy.prototype.handleClient = function(proxySocket) {
    var self = this;
    var key = uniqueKey(proxySocket);
    self.proxySockets[toString(key)] = proxySocket; // 存储这个socket的意义上是？
    // 打包一个 context 出来
    var context = {
        buffers: [],
        connected: false,
        proxySocket: proxySocket
    };
    proxySocket.on("data", function(data) {
        self.handleUpstreamData(context, data);
    });
    proxySocket.on("close", function(hadError) {
        delete self.proxySockets[uniqueKey(proxySocket)];
        if (context.serviceSocket !== undefined) {
            context.serviceSocket.destroy();
        }
    });
    proxySocket.on("error", function(e) {
        context.serviceSocket.destroy();
    });
};

// 处理往上游走的数据
TcpProxy.prototype.handleUpstreamData = function(context, data) {
    var self = this;
    // 是否已经连接到后端
    if (context.connected) {
        context.serviceSocket.write(
            self.intercept(self.options.upstream, context, data));
    } else {
        // 这里的buffers用处是什么？把数据存到 buffer 中
        context.buffers[context.buffers.length] =
            self.intercept(self.options.upstream, context, data);
        // 创建连接后端额socket
        if (context.serviceSocket === undefined) {
            self.createServiceSocket(context);
        }
    }
};

TcpProxy.prototype.createServiceSocket = function(context) {
    var self = this;
    var options = self.parseServiceOptions(context);
    if (self.options.tls === "both") {
        context.serviceSocket = tls.connect(options, function() {
            self.writeBuffer(context);
        });
    } else {
        context.serviceSocket = new net.Socket();
        context.serviceSocket.connect(options, function() {
            self.writeBuffer(context);
        });
    }
    context.serviceSocket.on("data", function(data) {
        context.proxySocket.write(
            self.intercept(self.options.downstream, context, data));
    });
    context.serviceSocket.on("close", function(hadError) {
        context.proxySocket.destroy();
    });
    context.serviceSocket.on("error", function(e) {
        context.proxySocket.destroy();
    });
};

TcpProxy.prototype.parseServiceOptions = function(context) {
    var self = this;
    var i = self.getServiceHostIndex(context.proxySocket);
    return Object.assign({
        port: self.servicePorts[parseInt(i, 10)],
        host: self.serviceHosts[parseInt(i, 10)],
        localAddress: self.options.localAddress,
        localPort: self.options.localPort
    }, self.serviceTlsOptions);
};

TcpProxy.prototype.getServiceHostIndex = function(proxySocket) {
    this.serviceHostIndex++;
    if (this.serviceHostIndex == this.serviceHosts.length) {
        this.serviceHostIndex = 0;
    }
    var index = this.serviceHostIndex;
    if (this.options.serviceHostSelected) {
        index = this.options.serviceHostSelected(proxySocket, index);
    }
    return index;
};

TcpProxy.prototype.writeBuffer = function(context) {
    context.connected = true;
    if (context.buffers.length > 0) {
        for (var i = 0; i < context.buffers.length; i++) {
            context.serviceSocket.write(context.buffers[parseInt(i, 10)]);
        }
    }
};

TcpProxy.prototype.end = function() {
    this.server.close();
    for (var key in this.proxySockets) {
        this.proxySockets[toString(key)].destroy();
    }
    this.server.unref();
};

TcpProxy.prototype.log = function(msg) {
    if (!this.options.quiet) {
        console.log(msg);
    }
};

TcpProxy.prototype.intercept = function(interceptor, context, data) {
    if (interceptor) {
        return interceptor(context, data);
    }
    return data;
};


// 启动代理
let createProxy = module.exports.createProxy;
let proxy = createProxy(8088, '127.0.0.1', 3000, {
    upstream: (context, data) => {
        process.stdout.write(data.toString());
        return data;
    },
    downstream: (context, data) => {
        process.stdout.write(data.toString());
        return data;
    },
});
process.on("uncaughtException", function(err) {
    console.error(err);
    proxy.end();
});
process.on("SIGINT", function() {
    proxy.end();
});
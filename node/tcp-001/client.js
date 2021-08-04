var connectTimeout = 5000
proto = require("http");
const agent = new proto.Agent({
  keepAlive: true,
  maxSockets: 1,
});

const data = JSON.stringify({todo:'xxx'});

function connect(h) {
    return new Promise((resolve, reject) => {
      const options = {
        hostname: 'dummy.host',
        port: 8080,
        path: '/gogo/',
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Content-Length': Buffer.from(data).length
        },
        agent: agent
      };
      let arr = h.split(':');
      options.host = arr[0];
      options.hostname = arr[0];
      options.port = h == arr[0] ? (options.https ? 443 : 80) : arr[1];
      options.headers['Host'] = "rio.zhejiang.zjnx.tcs2.fsphere.cn";
      options.headers['Accept'] = "*/*";
      options.headers['x-tif-timestamp'] = "1627549916";
      options.headers['x-tif-paasId'] = "acc";
      options.headers['x-tif-nonce'] = "345b6ac47f1a127b214d53fa4dc77e4b";
      options.headers['x-tif-signature'] = "8307eee1608cf94fe7901d5b14aa84d5bbc562f57e969241c04b5769f1d6d37d";
      options.headers['Content-Type'] = "application/json";
      options.headers['Cookie'] = "x_host_key=";
      options.headers['x-test-name'] = "test";
      let req = proto.request(options);
      req.setNoDelay();
      let timer = setTimeout(() => {
        console.log("timeout");
        req.removeListener('socket', onSocket);
        req.removeListener('error', onError);
        req.destroy();
        reject(Object.assign(new Error('send request timeout'), {
          address: options.hostname,
          port: options.port,
          code: 'ETIMEDOUT'
        }));
      }, connectTimeout);

      function onSocket(s) {
        if (!s.writable) {
          // http.Agent 的 socket 池有可能返回已经处于销毁状态的 socket
          onError(new Error('destroyed socket allocated'));
        } else if (s.remoteAddress) {
          onConnect();
        } else {
          s.once('connect', () => {
            s.setKeepAlive(true, 60000);
            onConnect();
          });
        }
      }
      function onError(err) {
        req.destroy();
        clearTimeout(timer);
        reject(err);
      }
      function onConnect() {
        req.removeListener('error', onError);
        clearTimeout(timer);
        req.on('close', () => {
          req.socket.emit('sp_request_close', Date.now());
        });
        // 如果连接成功，req 将被返回
        resolve(req);
      }
      req.once('socket', onSocket);
      req.once('error', onError);
    }).catch(err => {
      console.log("cached error", err);
    });
}

// setInterval(() => {
// }, 300);
function send() {
  socket = connect("127.0.0.1:8080");
  socket.then((req) => {
    req.write(data);
    req.end();
    req.on('response', (res) => {
      res.on('data', (chunk) => {
        console.log(`BODY: ${chunk}`);
      });
      res.on('end', () => {
        // console.log('No more data in response.');
      });
    });
  })
}

send(); send();
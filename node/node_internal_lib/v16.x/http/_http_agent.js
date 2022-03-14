// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

var net = require('net');
var util = require('util');
var EventEmitter = require('events').EventEmitter;
var debug = util.debuglog('http');

// New Agent code.

// The largest departure from the previous implementation is that
// an Agent instance holds connections for a variable number of host:ports.
// Surprisingly, this is still API compatible as far as third parties are
// concerned. The only code that really notices the difference is the
// request object.

// Another departure is that all code related to HTTP parsing is in
// ClientRequest.onSocket(). The Agent is now *strictly*
// concerned with managing a connection pool.

function Agent(options) {
  if (!(this instanceof Agent))
    return new Agent(options);

  EventEmitter.call(this);

  var self = this;

  self.defaultPort = 80;
  self.protocol = 'http:';

  self.options = util._extend({}, options);

  // don't confuse net and make it think that we're connecting to a pipe
  self.options.path = null;
  self.requests = {};
  self.sockets = {};
  self.freeSockets = {};
  self.keepAliveMsecs = self.options.keepAliveMsecs || 1000;
  self.keepAlive = self.options.keepAlive || false;
  self.maxSockets = self.options.maxSockets || Agent.defaultMaxSockets;
  self.maxFreeSockets = self.options.maxFreeSockets || 256;

  // 当 emit 事件 free 的时候。触发该情况的内容有:
  // 1. removeSocket 时。活跃的 socket 从 sockets 中移除，如果是已经 destroyed 的，在 freeSockets 中也是要移除的。
  self.on('free', function(socket, options) {
    var name = self.getName(options);
    debug('agent.on(free)', name);

    // 如果 socket 没有被 destroyed，且对应的request还在排队中
    if (!socket.destroyed &&
        self.requests[name] && self.requests[name].length) {
      // 选出一个 request 设置 socket 给他。
      self.requests[name].shift().onSocket(socket);
      
      // 如果已经没有request了。
      if (self.requests[name].length === 0) {
        // don't leak
        delete self.requests[name];
      }
    } else { // 如果 socket 没有被destroy，就直接free事件的话。
      // If there are no pending requests, then put it in
      // the freeSockets pool, but only if we're allowed to do so.
      var req = socket._httpMessage;
      if (req &&
          req.shouldKeepAlive &&
          !socket.destroyed &&
          self.options.keepAlive) { // option 设置了 keepalive 的选项。
        var freeSockets = self.freeSockets[name]; // 从 freesockets 中拿出 socket
        var freeLen = freeSockets ? freeSockets.length : 0;
        var count = freeLen; // 空闲的连接
        if (self.sockets[name]) // 如果 sockets 中有这个相关的连接正在用
          count += self.sockets[name].length;

        // 如果超过了允许的最大的 sockets或者空闲的超过了最大限制。将socket销毁掉
        if (count >= self.maxSockets || freeLen >= self.maxFreeSockets) {
          self.removeSocket(socket, options);
          socket.destroy();
        } else {
          // freeSockets，当前 name 的 freeSockets。将 socket 的keepAlaive设置好
          freeSockets = freeSockets || [];
          self.freeSockets[name] = freeSockets;
          socket.setKeepAlive(true, self.keepAliveMsecs);
          socket.unref(); // 不影响程序退出
          socket._httpMessage = null; // 断开原关联。
          self.removeSocket(socket, options); // 从 sockets 中移除这个 sockets。表示没有正在使用
          freeSockets.push(socket); // 添加 socket 到 freeSockets 中。
        }
      } else { // 不是设置了 keepAlive 选项。
        self.removeSocket(socket, options);
        socket.destroy();
      }
    }
  });
}

util.inherits(Agent, EventEmitter);
exports.Agent = Agent;

Agent.defaultMaxSockets = Infinity;

// 创建1个socket
Agent.prototype.createConnection = net.createConnection;

// 根据host和端口计算一个 key。例如 127.0.0.1:8877:
// Get the key for a given set of request options
Agent.prototype.getName = function(options) {
  var name = '';

  if (options.host)
    name += options.host;
  else
    name += 'localhost';

  name += ':';
  if (options.port)
    name += options.port;
  name += ':';
  if (options.localAddress)
    name += options.localAddress;
  name += ':';
  return name;
};

// 添加一个请求。req 是 HttpClientRequest 实例。将 req 和 socket 关联起来。没有分配 socket 的 req 放置到 request 队列中。options 是 httpClient 的 options。不是创建 agent 的 options。
Agent.prototype.addRequest = function(req, options) {
  // Legacy API: addRequest(req, host, port, path)
  if (typeof options === 'string') {
    options = {
      host: options,
      port: arguments[2],
      path: arguments[3]
    };
  }

  // 查看这个 key 对应的  sockets数组是否存在。不存在的话，初始化为一个新数组
  var name = this.getName(options);
  if (!this.sockets[name]) {
    this.sockets[name] = [];
  }

  // 查看这个 key 对应的 freeSockets 数组是否存在。不存在 freeLen 为 0 。
  var freeLen = this.freeSockets[name] ? this.freeSockets[name].length : 0;
  var sockLen = freeLen + this.sockets[name].length; // freeLen + sockets.length 得到当前的

  // 如果 freeLen > 0 表示有空闲的连接可以使用
  if (freeLen) {
    // 从数组头部取这个空闲的 sockets
    // we have a free socket, so use that.
    var socket = this.freeSockets[name].shift();
    debug('have free socket');

    // don't leak
    if (!this.freeSockets[name].length)
      delete this.freeSockets[name];

    // 正在使用中。不 unref 的话, 程序退出不了。
    socket.ref();
    // 给 req 设置 socket。
    req.onSocket(socket);
    // 将 socket 放入正在使用的队列
    this.sockets[name].push(socket);
  } else if (sockLen < this.maxSockets) { // 没有freeSocket的情况下, 如果 socketLen 也没有到达上限。则创建一个 socket
    debug('call onSocket', sockLen, freeLen);
    // If we are under maxSockets create a new one.
    req.onSocket(this.createSocket(req, options));
  } else {
    // 超过最大的 sockets 数量。将 req 加到 queue 中。先进先出
    debug('wait for socket');
    // We are over limit so we'll add it to the queue.
    if (!this.requests[name]) {
      this.requests[name] = [];
    }
    this.requests[name].push(req);
  }
};

// 创建一个新的socket。同时对socket的事件和agent的事件做联动。
Agent.prototype.createSocket = function(req, options) {
  var self = this;
  options = util._extend({}, options);
  options = util._extend(options, self.options);

  options.servername = options.host;
  if (req) {
    var hostHeader = req.getHeader('host');
    if (hostHeader) {
      options.servername = hostHeader.replace(/:.*$/, '');
    }
  }

  // 生成一个 key
  var name = self.getName(options);

  debug('createConnection', name, options);
  options.encoding = null;
  var s = self.createConnection(options); // 获取一个新的socket。命名为 s
  if (!self.sockets[name]) {
    self.sockets[name] = [];
  }
  this.sockets[name].push(s);
  debug('sockets', name, this.sockets[name].length);

  function onFree() {
    self.emit('free', s, options);
  }
  // socket 的 free 事件发生时。将 s 也传送 agent 的 free 事件。
  s.on('free', onFree);

  function onClose(err) {
    debug('CLIENT socket onClose');
    // This is the only place where sockets get removed from the Agent.
    // If you want to remove a socket from the pool, just close it.
    // All socket errors end in a close event anyway.
    self.removeSocket(s, options);
  }
  // socket 的 free 事件发生时。调用 agent 的 removeSocket。
  s.on('close', onClose);

  function onRemove() {
    // We need this function for cases like HTTP 'upgrade'
    // (defined by WebSockets) where we need to remove a socket from the
    // pool because it'll be locked up indefinitely
    debug('CLIENT socket onRemove');
    self.removeSocket(s, options);
    s.removeListener('close', onClose);
    s.removeListener('free', onFree);
    s.removeListener('agentRemove', onRemove);
  }
  // socket 的 agentRemove 事件发生时。调用 agent 的 removeSocket。
  s.on('agentRemove', onRemove);
  return s;
};

// 从sockets和freeSocket中移除 socket。如果 request 队列中还有request没有处理完。创建一个新的 socket 放到 freesocket。
Agent.prototype.removeSocket = function(s, options) {
  var name = this.getName(options);
  debug('removeSocket', name, 'destroyed:', s.destroyed);
  var sets = [this.sockets];

  // 如果 socket 是已经被 destoryed 的。则在 freeSocket 中的 socket 要清理掉。
  // If the socket was destroyed, remove it from the free buffers too.
  if (s.destroyed)
    sets.push(this.freeSockets);

  for (var sk = 0; sk < sets.length; sk++) {
    var sockets = sets[sk];

    if (sockets[name]) {
      var index = sockets[name].indexOf(s);
      if (index !== -1) {
        sockets[name].splice(index, 1);
        // Don't leak
        if (sockets[name].length === 0)
          delete sockets[name];
      }
    }
  }

  // 创建1个 freesocket。如果这个socket 对应的是有 request在队列中。free 事件是把 socket 放到 freeSocket 中。
  if (this.requests[name] && this.requests[name].length) {
    debug('removeSocket, have a request, make a socket');
    var req = this.requests[name][0];
    // If we have pending requests and a socket gets closed make a new one
    this.createSocket(req, options).emit('free');
  }
};

// 将 agent 管理的所有的连接 destroy 掉。
Agent.prototype.destroy = function() {
  var sets = [this.freeSockets, this.sockets];
  for (var s = 0; s < sets.length; s++) {
    var set = sets[s];
    var keys = Object.keys(set);
    for (var v = 0; v < keys.length; v++) {
      var setName = set[keys[v]];
      for (var n = 0; n < setName.length; n++) {
        setName[n].destroy();
      }
    }
  }
};

exports.globalAgent = new Agent();
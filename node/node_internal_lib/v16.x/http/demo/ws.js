const http = require('http');

// 创建 HTTP 服务器
const server = http.createServer((req, res) => {
    res.writeHead(200, { 'Content-Type': 'text/plain' });
    res.end('okay');
});
server.on('upgrade', (req, socket, head) => {
    console.log('trigger websocket');
    socket.write('HTTP/1.1 101 Web Socket Protocol Handshake\r\n' +
        'Upgrade: WebSocket\r\n' +
        'Connection: Upgrade\r\n' +
        '\r\n');
    socket.pipe(socket); // 回声
});

// 现在该服务器正在运行
server.listen(1337, '127.0.0.1', () => {
    // 发出请求
    const options = {
        port: 1337,
        host: '127.0.0.1',
        headers: {
            'Connection': 'Upgrade',
            'Upgrade': 'websocket'
        }
    };
    const req = http.request(options);
    req.end();
    req.on('upgrade', (res, socket, upgradeHead) => {
        console.log('got upgraded!');
        socket.write('hello world\n');
        socket.write('hello world\n');
        socket.write('hello world\n');
        socket.on('data', (d) => {
           console.log(d.toString());
        });
        socket.end();
        // process.exit(0);
    });
});
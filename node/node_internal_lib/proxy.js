const http = require('http');
const net = require('net');
const { URL } = require('url');

// 创建 HTTP 隧道代理
const proxy = http.createServer((req, res) => {
    res.writeHead(200, { 'Content-Type': 'text/plain' });
    res.end('okay');
});
proxy.on('connect', (req, clientSocket, head) => {
    // 连接到源服务器
    const { port, hostname } = new URL(`http://${req.url}`);
    const serverSocket = net.connect(port || 80, hostname, () => {
        clientSocket.write('HTTP/1.1 200 Connection Established\r\n' +
            'Proxy-agent: Node.js-Proxy\r\n' +
            '\r\n');
        serverSocket.write(head);
        serverSocket.pipe(clientSocket);
        clientSocket.pipe(serverSocket);
    });
});

proxy.listen(1337, '127.0.0.1', () => {
    console.log("proxy start on: 127.0.0.1:1337");
});



// 使用代理发送请求
// const options = {
//     port: 1337,
//     host: '127.0.0.1',
//     method: 'CONNECT',
//     path: 'www.google.com:80'
// };
// const req = http.request(options);
// req.end();
// req.on('connect', (res, socket, head) => {
//     console.log('got connected!');
//     // 通过 HTTP 隧道发出请求
//     socket.write('GET / HTTP/1.1\r\n' +
//         'Host: www.google.com:80\r\n' +
//         'Connection: close\r\n' +
//         '\r\n');
//     socket.on('data', (chunk) => {
//         console.log(chunk.toString());
//     });
//     socket.on('end', () => {
//         proxy.close();
//     });
// });
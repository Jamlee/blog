let h = "hello";
var net = require('net');
var server = net.createServer(function(socket) {
    let raw = [
        'HTTP/1.1 200 OK',
        'Content-Type: text/html; charset=UTF-8',
        'Content-Encoding: UTF-8',
        'Accept-Ranges: bytes',
        'Connection: keep-alive',
        'Content-Length: ' + h.length,
    ].join('\n') + '\n\n';
    socket.write(raw);
    socket.write(h);
    raw = [
        'HTTP/1.1 200 OK',
        'Content-Type: text/html; charset=UTF-8',
        'Content-Encoding: UTF-8',
        'Accept-Ranges: bytes',
        'Connection: keep-alive',
        'Content-Length: ' + h.length,
    ].join('\n') + '\n\n';
    socket.write(raw);
    socket.write(h);
});

server.listen(3000, '0.0.0.0');
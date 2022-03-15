const http = require('http');

const hostname = '0.0.0.0';
const port = 3000;

const server = http.createServer((req, res) => {
  console.log(JSON.stringify(req.headers));
  console.log(req.socket.remoteAddress + ":" + req.socket.remotePort);
  res.statusCode = 200;
  req.on('data', (chunked) => {
    console.log(chunked.toString());
  });
  req.on('end', () => {
    res.setHeader('Content-Type', 'text/plain');
    res.end('Hello, World!');
  });
});

server.listen(port, hostname, () => {
  console.log(`Server running at http://${hostname}:${port}/`);
});
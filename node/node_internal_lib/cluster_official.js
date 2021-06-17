const cluster = require('cluster');
const http = require('http');

if (cluster.isMaster) {
    console.log(`Primary ${process.pid} is running`);
    // Fork workers.
    for (let i = 0; i < 2; i++) {
        cluster.fork();
    }
    cluster.on('exit', (worker, code, signal) => {
        console.log(`worker ${worker.process.pid} died`);
    });
    console.log(`Master ${process.pid} started`);
} else {
    // Workers can share any TCP connection
    // In this case it is an HTTP server
    http.createServer((req, res) => {
        res.writeHead(200);
        res.end('hello world\n');
    }).listen(8010);

    console.log(`Worker ${process.pid} started`);
}
const server = require('net').createServer()
const cluster = require('cluster');
const path = require('path');

const workers = {}
cluster.setupMaster({
    exec: path.join(__dirname, "worker.js"),
});
for (i = 0; i < 2; i++) {
    const worker = cluster.fork()
    // 发送句柄给worker
    worker.send({name: "jamlee"}, server);
    workers[worker.pid] = worker
}
// 执行完毕也不会退出的
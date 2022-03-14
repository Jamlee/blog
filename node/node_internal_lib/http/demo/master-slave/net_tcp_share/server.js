const { fork } = require('child_process')
const server = require('net').createServer()

// master监听8888
server.listen(8888, () => {
    console.log('master on :', 8888)
})

const workers = {}
for (i = 0; i < 2; i++) {
    const worker = fork('./worker.js')
    // 发送句柄给worker
    worker.send('server', server)
    workers[worker.pid] = worker
}
// console.log(workers);
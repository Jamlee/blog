// worker.js
const http = require('http')
const server = http.createServer((req, res) => {
    res.end('hahahaha');
}) // 不监听端口
server.listen(3000);

process.on('message', (msg) => {
    if (msg === 'server') {
    }
})

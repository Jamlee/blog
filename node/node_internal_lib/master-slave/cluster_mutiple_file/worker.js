// worker.js
const http = require('http')
const server = http.createServer((req, res) => {
    res.end('hahahaha')
});
server.listen(9098);
process.on('message', (msg) => {
    if (msg === 'server') {
        console.log(msg);
    }
})

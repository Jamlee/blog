# -*- coding: UTF-8 -*-

from collections import deque
from select import select
 
class YieldEvent:
    def handle_yield(self, sched, task):
        pass
 
    def handle_resume(self, sched, task):
        pass
 
# 任务调度(相当于EventLoop)
class Scheduler:
    def __init__(self):
        self._numtasks = 0         # 任务总数量
        self._ready = deque()      # 等待执行的任务队列
        self._read_waiting = {}    # 正等待读的任务
        self._write_waiting = {}   # 正等待写的任务
 
    # 利用I/O多路复用 监听读写I/0。将 task 和 io 进行关联。
    def _iopoll(self):
        rset, wset, eset = select(self._read_waiting,
                                  self._write_waiting, [])
        for r in rset:
            evt, task = self._read_waiting.pop(r)
            evt.handle_resume(self, task)
        for w in wset:
            evt, task = self._write_waiting.pop(w)
            evt.handle_resume(self, task)
 
    def new(self, task):
        """添加一个新的任务"""
        self._ready.append((task, None))
        self._numtasks += 1
 
    def add_ready(self, task, msg=None):
        """添加到任务对列等待执行"""
        self._ready.append((task, msg))
 
    def _read_wait(self, fileno, evt, task):
        self._read_waiting[fileno] = (evt, task)
 
    def _write_wait(self, fileno, evt, task):
        self._write_waiting[fileno] = (evt, task)
 
    def run(self):
        while self._numtasks:
            # 如果任务数量为空，阻塞在select处，保持监听
            if not self._ready:
                self._iopoll()
            task, msg = self._ready.popleft()
            try:
                r = task.send(msg)
                if isinstance(r, YieldEvent):
                    r.handle_yield(self, task)
                else:
                    raise RuntimeError('unrecognized yield event')
            except StopIteration:
                self._numtasks -= 1
 
# 示例： 将协程抽象成YieldEvent的子类，并重写handle_yield和handle_resume方法
class ReadSocket(YieldEvent):
    def __init__(self, sock, nbytes):
        self.sock = sock
        self.nbytes = nbytes
 
    def handle_yield(self, sched, task):
        sched._read_wait(self.sock.fileno(), self, task)
 
    def handle_resume(self, sched, task):
        data = self.sock.recv(self.nbytes)
        sched.add_ready(task, data)
 
class WriteSocket(YieldEvent):
    def __init__(self, sock, data):
        self.sock = sock
        self.data = data
 
    def handle_yield(self, sched, task):
        sched._write_wait(self.sock.fileno(), self, task)
 
    def handle_resume(self, sched, task):
        nsent = self.sock.send(self.data)
        sched.add_ready(task, nsent)
 
class AcceptSocket(YieldEvent):
    def __init__(self, sock):
        self.sock = sock
 
    def handle_yield(self, sched, task):
        sched._read_wait(self.sock.fileno(), self, task)
 
    def handle_resume(self, sched, task):
        r = self.sock.accept()
        sched.add_ready(task, r)
 
 
class Socket(object):
    def __init__(self, sock):
        self._sock = sock
 
    def recv(self, maxbytes):
        return ReadSocket(self._sock, maxbytes)
 
    def send(self, data):
        return WriteSocket(self._sock, data)
 
    def accept(self):
        return AcceptSocket(self._sock)
 
    def __getattr__(self, name):
        return getattr(self._sock, name)
 
if __name__ == '__main__':
    from socket import socket, AF_INET, SOCK_STREAM
 
    def readline(sock):
        chars = []
        while True:
            c = yield sock.recv(1)
            print(c)
            if not c:
                break
            chars.append(c)
            if c == b'\n':
                break
        return b''.join(chars)
 
    # socket server 使用生成器
    class EchoServer:
        def __init__(self, addr, sched):
            self.sched = sched
            sched.new(self.server_loop(addr))
 
        def server_loop(self, addr):
            s = Socket(socket(AF_INET, SOCK_STREAM))
            s.bind(addr)
            s.listen(5)
            while True:
                c, a = yield s.accept()
                print('Got connection from ', a)
                print("got", c)
                self.sched.new(self.client_handler(Socket(c)))
 
        def client_handler(self, client):
            while True:
                try:
                    line = yield from readline(client)
                    if not line:
                        break
 
                    print("from Client::", str(line))
                except Exception:
                    break
 
                while line:
                    try:
                        nsent = yield client.sendall(line)
                        print("nsent", nsent)
                        line = line[nsent:]
                    except Exception:
                        break
            client.close()
            print('Client closed')
 
    sched = Scheduler()
    EchoServer(('localhost', 9999), sched)
    sched.run()
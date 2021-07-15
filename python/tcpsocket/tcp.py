# -*- coding: UTF-8 -*-
# 【1】Python中Socket的Close方法假关闭Socket连接的问题 https://blog.csdn.net/ztb3214/article/details/17405385
# 【2】https://stackoverflow.com/questions/48024720/python-how-to-check-if-socket-is-still-connected
import socketserver
import os
import time
import signal
import sys
import socket

exit_flag = False

class MyTCPHandler(socketserver.BaseRequestHandler):
    def is_socket_closed(self, sock: socket.socket) -> bool:
        try:
            # this will try to read bytes without blocking and also without removing them from buffer (peek only)
            data = sock.recv(16, socket.MSG_DONTWAIT | socket.MSG_PEEK)
            if len(data) == 0:
                return True
        except BlockingIOError:
            return False  # socket is open and reading from it would block
        except ConnectionResetError:
            return True  # socket was closed for some other reason
        except Exception as e:
            return False
        return False

    """
    The request handler class for our server.

    It is instantiated once per connection to the server, and must
    override the handle() method to implement communication to the
    client.
    """
    def handle(self):
        self.request.settimeout(1)
        # self.request is the TCP socket connected to the client
        while 1:
            if exit_flag:
                try:
                    self.request.shutdown(2)
                except: pass
                self.request.close()
                print ("退出线程2")
                break
            try:
                self.data = self.request.recv(1024).strip()
                if len(self.data) > 0:
                    print("{} wrote:".format(self.client_address[0]))
                    print(self.data)
                    # just send back the same data, but upper-cased
                    self.request.sendall(self.data.upper())
                else:
                    # 如果链接已经关闭
                    if self.is_socket_closed(self.request):
                        try:
                            self.request.shutdown(2)
                        except: pass
                        self.request.close()
                        print ("退出线程1")
                        break
            except Exception as e:
                pass

class ThreadedTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
        pass

arrs = []
def signal_handler(sig, frame):
    print('You pressed Ctrl+C!')
    global exit_flag
    exit_flag = True
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

if __name__ == "__main__":
    print('pid:' + str(os.getpid()))
    HOST, PORT = "0.0.0.0", 9999
    # Create the server, binding to localhost on port 9999
    with ThreadedTCPServer((HOST, PORT), MyTCPHandler) as server:
        # Activate the server; this will keep running until you
        # interrupt the program with Ctrl-C
        server.serve_forever()

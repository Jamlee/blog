# -*- coding: UTF-8 -*-
import socket
import threading
import socket
import signal
import sys

exit_flag = False

class myThread (threading.Thread):
    def __init__(self, threadID, name):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
    
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

    def run(self):
        print ("开始线程：" + self.name)
        # ceate an ipv4 (AF_INET) socket object using the tcp protocol (SOCK_STREAM)
        conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        conn.connect(('192.168.33.14', 9999))
        conn.send(b'hello')
        conn.settimeout(1)
        while 1:
            if exit_flag:
                try:
                    conn.shutdown(2)
                except: pass
                conn.close()
                print ("退出线程1：" + self.name)
                break
            # recv 是阻塞的
            try:
                if self.is_socket_closed(conn):
                    print ("退出线程2：" + self.name)
                    try:
                        conn.shutdown(2)
                    except: pass
                    conn.close()
                    break
                conn.recv(1024)
            except:
                pass

arrs = []
def signal_handler(sig, frame):
    print('You pressed Ctrl+C!')
    global exit_flag
    exit_flag=True
    sys.exit(0)  
signal.signal(signal.SIGINT, signal_handler)

try:
    i=1
    while(i<=100):
        t = myThread(i, "Thread-" + str(i))
        t.setDaemon(False)
        t.start()
        arrs.append(t)
        i+=1
    while not exit_flag:
        pass
except Exception as e:
    print(e)
    print("Error: unable to start thread")

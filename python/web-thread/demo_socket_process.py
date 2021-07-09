# 多进程如何共享socket
import socket
import multiprocessing


def func(cnn):
    while True:
        try:
            request = cnn.recv(1024)
            print(request)
            cnn.send(b'%s' % request)

        except ConnectionResetError:
            cnn.close()
            break


if __name__ == '__main__':
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(('0.0.0.0', 4444))
    s.listen(5)
    while True:
        try:
            print("开始监听")
            cnn, addr = s.accept()
            m = multiprocessing.Process(target=func, args=(cnn,))
            m.daemon = True  # daemon True设置为守护即主死子死.
            m.start()  # 开启一个子进程, func中的while 来接受cnn后续内容.

        except ConnectionResetError:
            pass

        except Exception as e:
            print(e)
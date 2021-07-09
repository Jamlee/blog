# -*- coding: UTF-8 -*-

from threading import Thread
from threading import Lock
import time
 
# def add_one(a):
#     time.sleep(1)
#     # print("in thread a:", a)
#     a[0] += 1
 
# if __name__ == '__main__':
#     array = [0]
#     thread_obj_list = []
#     for i in range(50):
#         t = Thread(target=add_one, args=(array,))
#         t.start()
#         thread_obj_list.append(t)
#     for j in thread_obj_list:
#         j.join()
#     print("array result::", array[0])

# python2 modify_one_var.py
# ('array result::', 32)
# python3 modify_one_var.py
# ('array result::', 50)

lock = Lock()
def add_one(a):
    time.sleep(1)
    lock.acquire()
    a[0] += 1
    lock.release()
 
if __name__ == '__main__':
    array = [0]
    thread_obj_list = []
    for i in range(50):
        t = Thread(target=add_one, args=(array,))
        t.start()
        thread_obj_list.append(t)
    for j in thread_obj_list:
        j.join()
    print("array result::", array[0])
# python2 modify_one_var.py
# ('array result::', 50)
# https://www.cnblogs.com/zingp/p/8678109.html
# -*- coding: UTF-8 -*-
from collections import deque
 
def sayHello(n):
    while n > 0:
        print("hello~", n)
        yield n
        n -= 1
    print('say hello')
 
def sayHi(n):
    x = 0
    while x < n:
        print('hi~', x)
        yield
        x += 1
    print("say hi")
 
# 使用yield语句，实现简单任务调度器
class TaskScheduler(object):
    def __init__(self):
        self._task_queue = deque()
 
    def new_task(self, task):
        '''
        向调度队列添加新的任务
        '''
        self._task_queue.append(task)
 
    def run(self):
        '''
        不断运行，直到队列中没有任务
        '''
        while self._task_queue:
            task = self._task_queue.popleft()
            try:
                next(task)
                self._task_queue.append(task)
            except StopIteration:
                # 生成器结束
                pass
 
sched = TaskScheduler()
sched.new_task(sayHello(10))
sched.new_task(sayHi(15))
sched.run()
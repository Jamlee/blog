import time

def consume():
    r = ''
    while True:
        n = yield r
        if not n:
            return
        print('[consumer] consuming %s...' % n)
        time.sleep(1)
        r = 'well received'

def produce(c):
    next(c)
    n = 0
    while n < 5:
        n = n + 1
        print('[producer] producing %s...' % n)
        r = c.send(n)
        print('[producer] consumer return: %s' % r)
    c.close()

if __name__=='__main__':
    c = consume()
    produce(c)

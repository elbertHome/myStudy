#!/usr/local/bin/python
# -*- coding: UTF-8 -*-

from random import randint
from time import sleep
from Queue import Queue
from myThread import MyThread


def writeQ(queue):
    print "producing object for Q ...",
    queue.put("xxx", True)
    print "size now ", queue.qsize()


def readQ(queue):
    val = queue.get(True)
    print "consumed object [%r] from Q ... size now" % val, queue.qsize()


def writer(queue, loops):
    for i in range(loops):
        writeQ(queue)
        sleep(randint(1, 3))


def reader(queue, loops):
    for i in range(loops):
        readQ(queue)
        sleep(randint(2, 5))

funcs = [writer, reader]
nfuncs = range(len(funcs))


def main():
    nloops = randint(2, 100)
    q = Queue(32)
    threads = []

    for i in nfuncs:
        t = MyThread(funcs[i], (q, nloops), funcs[i].__name__)
        threads.append(t)

    for i in nfuncs:
        threads[i].start()

    for i in nfuncs:
        threads[i].join()

    print "All Done"


if __name__ == "__main__":
    main()









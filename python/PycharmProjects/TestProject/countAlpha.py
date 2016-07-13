#!/usr/local/bin/python
# -*- coding: UTF-8 -*-

from os.path import getsize
from myThread import MyThread
from math import ceil

filepath = "/home/ium/myStudy/python/PycharmProjects/data/countAlpha.data"


def countalpha1(curChar):
    datafile = file(filepath, "r")
    sumCount = datafile.read().count(curChar)
    datafile.close()
    return sumCount


def countalpha2(curChar, start, size):
    datafile = file(filepath, "r")
    datafile.seek(start)
    sumCount = datafile.read(size).count(curChar)
    datafile.close()
    return sumCount


def lmtCountalpha(curChar):
    maxThreadCount = 6
    filesize = getsize(filepath)
    partsize = filesize // maxThreadCount + 1
    threads = []
    for i in range(maxThreadCount):
        start = i * partsize
        t = MyThread(countalpha2, (curChar, start, partsize), countalpha2.__name__)
        threads.append(t)

    for i in range(maxThreadCount):
        threads[i].start()

    for i in range(maxThreadCount):
        threads[i].join()

    return sum([threads[i].getResult() for i in range(maxThreadCount)])


def main(curChar):
    print "ST: There is %d [%s] in file [%s]" % (countalpha1(curChar), curChar, filepath)
    print "MT: There is %d [%s] in file [%s]" % (mtCountalpha(curChar), curChar, filepath)

if __name__ == "__main__":
    char = "html"
    main(char)
    import timeit
    print "single thread used ",
    print timeit.timeit("countalpha1('%s')" % char, setup="from __main__ import countalpha1", number=100), " sec\n"
    print "muliti thread used ",
    print timeit.timeit("mtCountalpha('%s')" % char, setup="from __main__ import mtCountalpha", number=100), " sec"

#!/usr/local/bin/python
# -*- coding: UTF-8 -*-

from atexit import register
from random import randrange
from threading import Thread, currentThread, Lock
from time import ctime, sleep


class CleanOutputSet(set):
    def __str__(self):
        return ", ".join(x for x in self)

lock = Lock()
loops = (randrange(2, 5) for x in xrange(randrange(6, 10)))
remaining = CleanOutputSet()


def loop(nsec):
    myname = currentThread().getName()
    lock.acquire()
    remaining.add(myname)
    print "[%s] started %s" % (ctime(), myname)
    lock.release()
    sleep(nsec)
    lock.acquire()
    remaining.remove(myname)
    print "[%s] Completed %s (%d secs)" % (ctime(), myname, nsec)
    print "    (remaining: %s)" % (remaining or "NONE")
    lock.release()


def main():
    for pause in loops:
        Thread(target=loop, args=(pause, )).start()


@register
def _atexit():
    print "All Done at:", ctime()

if __name__ == "__main__":
    main()

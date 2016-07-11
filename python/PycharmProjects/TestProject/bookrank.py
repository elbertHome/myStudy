#!/usr/local/bin/python
# -*- coding: UTF-8 -*-

from atexit import register
from re import compile
from threading import Thread
from time import ctime
import urllib2
import ssl

REGEX = compile("#([\d,]+) in Books")
AMZN = "https://www.amazon.com/dp/"
ISBNS = {
    "0132269937": "Core Python Programing",
    "0132356139": "Python Web Development with Django",
    "0137143419": "Python Fundamentals"
}


def getRanking(isbn):
    ssl._create_default_https_context = ssl._create_unverified_context
    curUrl = "%s%s" % (AMZN, isbn)
    print curUrl
    opener = urllib2.build_opener(
        urllib2.HTTPHandler(),
        urllib2.HTTPSHandler(),
        urllib2.ProxyHandler({'https': 'web-proxy.chn.hp.com:8080',
                              'http': 'web-proxy.chn.hp.com:8080'}))
    urllib2.install_opener(opener)
    page = urllib2.urlopen(curUrl)
    data = page.read()
    page.close()
    return REGEX.findall(data)[0]


def _showRanking(isbn):
    print "- %r ranked %s" % (ISBNS[isbn], getRanking(isbn))


def main():
    print "AT", ctime(), "on Amazon..."
    for isbn in ISBNS:
        # _showRanking(isbn)
        Thread(target=_showRanking, args=(isbn,)).start()


@register
def _atexit():
    print "all Done at:", ctime()


if __name__ == "__main__":
    main()

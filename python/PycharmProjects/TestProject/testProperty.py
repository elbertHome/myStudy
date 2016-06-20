#!/usr/local/bin/python
# -*- coding: UTF-8 -*-

class HideX(object):
    def __init__(self, val):
        self.__x = val

    @property
    def x(self):
        return self.__x

    @x.setter
    def x(self, val):
        self.__x = val

ins = HideX(12)
print "instance dir"
for x in dir(ins):
    print x
print "\n\n"
print "instance __dict__"
for x in ins.__dict__.items():
    print x
print "\n\n"

print "class dir", dir(HideX)
for x in dir(HideX):
    print x
print "\n\n"

print "class __dict__"
for x in HideX.__dict__.items():
    print x

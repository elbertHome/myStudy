#!/usr/local/bin/python
# -*- coding: UTF-8 -*-

class SortedKeyDict(dict):
    def keys(self):
        return sorted(super(SortedKeyDict, self).keys())
        # return sorted(self.keys())


def fibb():
    fibs = [0, 1]
    num = input("how many fibonacci numbers do you want ?")
    for i in xrange(num-2):
        fibs.append(fibs[-1] + fibs[-2])
    return fibs


def main():
    d = SortedKeyDict({('zheng-cai', 67), ('hui-jun', 68), ('xin-yi', 2)})
    print 'By iterator'
    print [key for key in d]
    print 'By keys'
    print fibb()


if __name__ == "__main__":
    main()

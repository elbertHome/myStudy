#!/usr/local/bin/python
# -*- coding: UTF-8 -*-

alp = {1: ["I", "V", "X"],
       10: ["X", "L", "C"],
       100: ["C", "D", "M"],
       1000: ["M", "", ""]}


def checkio(data):
    level = 1000
    rtnStr = ""
    while data > 0:
        num = data // level
        if num != 0:
            data -= level * num
            rtnStr += convert(level, num)
        level /= 10
    # replace this for solution
    return rtnStr


def convert(level, num):
    if 1 <= num <= 3:
        rtnStr = alp[level][0] * num
    elif num == 4:
        rtnStr = alp[level][0] + alp[level][1]
    elif 5 <= num <= 8:
        rtnStr = alp[level][1] + (alp[level][0] * (num - 5))
    elif num == 9:
        rtnStr = alp[level][0] + alp[level][2]
    else:
        rtnStr = ""
    return rtnStr


if __name__ == '__main__':
    # These "asserts" using only for self-checking and not necessary for auto-testing
    assert checkio(6) == 'VI', '6'
    assert checkio(76) == 'LXXVI', '76'
    assert checkio(499) == 'CDXCIX', '499'
    assert checkio(3888) == 'MMMDCCCLXXXVIII', '3888'
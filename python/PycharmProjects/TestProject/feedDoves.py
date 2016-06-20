#!/usr/local/bin/python
# -*- coding: UTF-8 -*-


def checkio(foodCount):
    rslt = 0
    for curMin in xrange(1, 2000):
        sumDove = curMin * (curMin + 1) / 2
        foodCount -= sumDove
        if foodCount < 0:
            rslt = sumDove - (curMin if abs(foodCount) >= curMin else abs(foodCount))
            break
        elif foodCount == 0:
            rslt = sumDove
            break

    return rslt


if __name__ == '__main__':
    # These "asserts" using only for self-checking and not necessary for auto-testing
    assert checkio(117) == 33, "1st example"
    assert checkio(2) == 1, "2nd example"
    assert checkio(5) == 3, "3rd example"
    assert checkio(10) == 6, "4th example"
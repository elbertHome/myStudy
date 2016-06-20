#!/usr/local/bin/python
# -*- coding: UTF-8 -*-


def check_connection(network, first, second):
    listSets = []

    for item in network:
        node1, node2 = item.split("-")
        setNode1 = None
        setNode2 = None
        for curset in listSets:
            if node1 in curset:
                setNode1 = curset
            if node2 in curset:
                setNode2 = curset
        if setNode1 == setNode2 is None:
            setTemp = set()
            setTemp.add(node1)
            setTemp.add(node2)
            listSets.append(setTemp)
        elif setNode1 is None and setNode2 is not None:
            setNode2.add(node1)
        elif setNode1 is not None and setNode2 is None:
            setNode1.add(node2)
        elif setNode1 is not None and setNode2 is not None and setNode1 is not setNode2:
            setTemp = setNode1.union(setNode2)
            listSets.remove(setNode1)
            listSets.remove(setNode2)
            listSets.append(setTemp)
        else:
            pass

    print listSets
    boolisFound = False
    for curset in listSets:
        if first in curset and second in curset:
            boolisFound = True
            break

    return boolisFound


if __name__ == '__main__':
    # These "asserts" using only for self-checking and not necessary for auto-testing
    assert check_connection(
        ("dr101-mr99", "mr99-out00", "dr101-out00", "scout1-scout2",
         "scout3-scout1", "scout1-scout4", "scout4-sscout", "sscout-super"),
        "scout2", "scout3") == True, "Scout Brotherhood"
    assert check_connection(
        ("dr101-mr99", "mr99-out00", "dr101-out00", "scout1-scout2",
         "scout3-scout1", "scout1-scout4", "scout4-sscout", "sscout-super"),
        "super", "scout2") == True, "Super Scout"
    assert check_connection(
        ("dr101-mr99", "mr99-out00", "dr101-out00", "scout1-scout2",
         "scout3-scout1", "scout1-scout4", "scout4-sscout", "sscout-super"),
        "dr101", "sscout") == False, "I don't know any scouts."
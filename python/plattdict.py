#!/usr/bin/python
def flatdict(dictData, path, dictRst):
    for curkey, curval in dictData.items():
        if isinstance(curval, dict):
            if curval:
                flatdict(curval, path + curkey + "/", dictRst)
            else:
                dictRst[path + curkey] = ""
        else:
            dictRst[path + curkey] = curval


def main():
    # data = {"a":{"b":{"c":{"d":"value"}}}}
    # data = {"empty": {}}
    data = {"name": {
        "first": "One",
        "last": "Drone"
    },
        "job": "scout",
        "recent": {},
        "additional": {
            "place": {
                "zone": "1",
                "cell": "2"}}}
    rstData = {}
    flatdict(data, "", rstData)
    print rstData


if __name__ == "__main__":
    main()

#!/usr/bin/python


def flatdict(dictData):
    keyList = dictData.keys()
    for curkey in keyList:
        curval = dictData.pop(curkey)
        if isinstance(curval, dict):
            subkey, subval = flatdict(curval)
            return curkey + "/" + subkey, subval
        else:
            return curkey, curval


def main():
    data = {"a":{"b":{"c":{"d":"value"}}}}
    keyList = data.keys()
    for key in keyList:
        val = data.pop(key)
        if isinstance(val, dict):
            rtnkey, rtnval = flatdict(val)
            data[key + "/" + rtnkey] = rtnval
                
    print data

if __name__ == "__main__":
    main()

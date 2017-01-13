#!/usr/bin/python
# -*- coding: UTF-8 -*-
import telnetlib
import DirOps

TELNET_HOST = "15.210.144.101"
UPDATE_SQL_STR = "update =EAS_MSG_MOVE_STATUS_TB set status=1 where login_name ='%s';"


@DirOps.decoFun
def updateNsimStatus(mailLoginId):
    tn = telnetlib.Telnet(TELNET_HOST)
    print tn.read_until("Enter Choice> ")
    tn.write("TACL\r\n")
    print tn.read_until("1> ")
    tn.write("LOGON super.st1\r\n")
    print tn.read_until("Password: ")
    tn.write("st1\r\n")
    print tn.read_until("ATMAIL 1>")
    tn.write("sqlci\r\n")
    print tn.read_until(">>")
    tn.write(UPDATE_SQL_STR % mailLoginId + "\r\n")
    print tn.read_until(">>")
    tn.write("exit\r\n")
    print tn.read_until("> ")
    tn.write("logoff\r\n")
    tn.close()


def test():
    updateNsimStatus("DDCZ0000000100")

if __name__ == "__main__":
    test()

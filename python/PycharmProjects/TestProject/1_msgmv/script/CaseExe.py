#!/usr/bin/python
# -*- coding: UTF-8 -*-
import os
import time
import DirOps
import DBOps
import MailOps
import NonStopOps

# gloabl vars
# Case Number
CASE_NO = "1-001"
# the login id of migrating
LOGIN_ID = "DDCZ0000000100"
MBS_ID = "rzm03box82.ezweb.ne.jp"
NSIM_ID = "001"
SMTP_ID = "001"
# status for restart
QUE_PROC_STATUS = 0
QUE_IMAP_STATUS = 0
QUE_SMTP_STATUS = 0
PLAN_PROC_STATUS = 0
PLAN_MAIL_MOVE_FLAG = 1
PLAN_DATA_MOVE_FLAG = 0
PLAN_OPERATION_FLAG = 0
# config file path of msgmv
# MSGMV_CONFIG_PATH = os.getcwd() + "/../conf/msgmv.conf"
# DB_PASSWORD = "LDuud_4vrQvZ.pQ6mseOSW6hMzS0r9I"
MSGMV_CONFIG_PATH = "/opt/zimbra/wk_ddc2/1_msgmv/conf/msgmv.conf"
DB_PASSWORD = "bizdb"
LD_LIBRARY_PATH = "/opt/zimbra/wk_ddc2/msgmv_for_compile/lib" \
                  ":/opt/zimbra/wk_ddc2/msgmv_for_compile/lib/mysql" \
                  ":/opt/zimbra/wk_ddc2/msgmv_for_compile/lib/openssl"
VALGRIND_CMD = "valgrind --tool=memcheck --leak-check=full"
EAS_DEFAULT_PASSWORD = "password00"
CONTACT_FILE_NAME = "contactTest.vcf"
CALENDAR_FILE_NAME = "calendarTest.ics"


@DirOps.decoFun
def loadConfig(envDic, configPath, splitStr=None, keyAdditional=""):
    fp = open(configPath, "r")
    for eachline in fp:
        eachline = eachline.strip()
        if eachline.startswith("#") \
                or len(eachline) == 0:
            continue
        spltList = eachline.split(splitStr)
        key = spltList[0].strip()
        if splitStr is not None:
            val = splitStr.join(spltList[1:])
        else:
            val = "".join(spltList[1:])
        envDic[keyAdditional + str(key)] = str(val)
    fp.close()
    print "%s was loaded" % configPath


@DirOps.decoFun
def loadConfigs():
    envDic = dict()
    curTime = time.localtime()
    # insert global vars into env
    envDic["CASE_NO"] = CASE_NO
    envDic["LOGIN_ID"] = LOGIN_ID
    envDic["MBS_ID"] = MBS_ID
    envDic["NSIM_ID"] = NSIM_ID
    envDic["SMTP_ID"] = SMTP_ID
    envDic["MSGMV_CONFIG_PATH"] = MSGMV_CONFIG_PATH
    envDic["MBS_IMAP_PORT"] = "7143"
    envDic["DB_PASSWORD"] = DB_PASSWORD
    envDic["QUE_PROC_STATUS"] = QUE_PROC_STATUS
    envDic["QUE_IMAP_STATUS"] = QUE_IMAP_STATUS
    envDic["QUE_SMTP_STATUS"] = QUE_SMTP_STATUS
    envDic["PLAN_MAIL_ADDR_EXTRA"] = LOGIN_ID + "@ezweb.ne.jp"
    envDic["PLAN_MAIL_ADDR"] = LOGIN_ID + "@ezweb.ne.jp"
    envDic["PLAN_PROC_STATUS"] = PLAN_PROC_STATUS
    envDic["PLAN_MAIL_MOVE_FLAG"] = PLAN_MAIL_MOVE_FLAG
    envDic["PLAN_DATA_MOVE_FLAG"] = PLAN_DATA_MOVE_FLAG
    envDic["EXE_TIME"] = time.strftime("%Y-%m-%d %H:%M:%S", curTime)
    envDic["EXE_TIME2"] = time.strftime("%Y%m%d%H%M%S", curTime)
    envDic["LD_LIBRARY_PATH"] = LD_LIBRARY_PATH
    envDic["VALGRIND_CMD"] = VALGRIND_CMD
    envDic["PLAN_OPERATION_FLAG"] = PLAN_OPERATION_FLAG
    envDic["EAS_DEFAULT_PASSWORD"] = EAS_DEFAULT_PASSWORD
    envDic["CONTACT_FILE_NAME"] = CONTACT_FILE_NAME
    envDic["CALENDAR_FILE_NAME"] = CALENDAR_FILE_NAME

    loadConfig(envDic, MSGMV_CONFIG_PATH, splitStr="=")
    loadConfig(envDic, envDic["MBS_HOST_FILEPATH"], keyAdditional="EASLIST_")
    loadConfig(envDic, envDic["NSIM_HOST_FILEPATH"], keyAdditional="NSIMLIST_")
    loadConfig(envDic, envDic["SMTP_HOST_FILEPATH"], keyAdditional="SMTPLIST_")
    print "All configs were loaded"
    return envDic


def main():
    # stdout redirect
    import sys
    originStdout = sys.stdout
    originStderr = sys.stderr
    try:
        logfilePath = os.getcwd() + "/../log/CaseExe.log"
        logfile = open(logfilePath, "wb")
        sys.stdout = logfile
        sys.stderr = logfile
        print "=====main() start====="
        envDic = loadConfigs()
        DirOps.iniDir(envDic)
        NonStopOps.updateNsimStatus(envDic["LOGIN_ID"])
        DBOps.prepareDBdata(envDic)
        MailOps.prepareMailData(envDic)
        MailOps.prepareContact(envDic)
        MailOps.prepareCalendar(envDic)
        MailOps.executeMsgmv(envDic, withValgrind=True)
        MailOps.checkMailData(envDic)
        DBOps.checkDBData(envDic)
        DirOps.saveAllEvidence(envDic)
        print "=====main() end======="
        logfile.close()
        backupFilePath = os.getcwd() + "/../evidence/%s-%s/log/CaseExe.log" \
                                       % (envDic["CASE_NO"], envDic["EXE_TIME2"])
        os.popen("cp %s %s -f" % (logfilePath, backupFilePath))
    except Exception as e:
        print "error occured: %s" % e
        print "failed to main"
    finally:
        # stdout redirect back
        sys.stdout = originStdout
        sys.stderr = originStderr


if __name__ == "__main__":
    main()

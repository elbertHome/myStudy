#!/usr/bin/python
# -*- coding: UTF-8 -*-
import os
import DirOps


@DirOps.decoFun
def executePerlScript(filePath, loginID, msg):
    rslt = os.popen("perl %s %s" % (filePath, loginID))
    for eachLine in rslt:
        print eachLine
    print "%s by %s" % (msg, filePath)


@DirOps.decoFun
def prepareMailData(envDic):
    curDir = os.getcwd()
    executePerlScript(curDir + "/ClearData_NSIM.pl", envDic["LOGIN_ID"], "NSIM Mail,Folder were cleared")
    executePerlScript(curDir + "/importData_MBS.pl", envDic["LOGIN_ID"], "MBS Account,Mail,Folder were created")
    executePerlScript(curDir + "/importData_NSIM.pl", envDic["LOGIN_ID"], "NSIM Mail,Folder were created")


@DirOps.decoFun
def prepareContact(envDic):
    curDir = os.getcwd()
    contactFilePath = curDir + "/" + envDic["CONTACT_FILE_NAME"]
    urlPath = envDic["MBS_REST_URL_PREFIX"] + envDic["EASLIST_" + envDic["MBS_ID"]] \
        + ":" + envDic["MBS_REST_PORT_NO"] + envDic["MBS_REST_URL_SUFFIX_CONTACTS"]

    cmd = "curl -u %s:%s --upload-file %s %s" % \
          (envDic["LOGIN_ID"],
           envDic["EAS_DEFAULT_PASSWORD"],
           contactFilePath,
           urlPath)
    print "concat cmd = %s" % cmd
    rslt = os.popen(cmd)
    for eachLine in rslt:
        print eachLine


@DirOps.decoFun
def prepareCalendar(envDic):
    curDir = os.getcwd()
    calendarFilePath = curDir + "/" + envDic["CALENDAR_FILE_NAME"]
    urlPath = envDic["MBS_REST_URL_PREFIX"] + envDic["EASLIST_" + envDic["MBS_ID"]] \
        + ":" + envDic["MBS_REST_PORT_NO"] + envDic["MBS_REST_URL_SUFFIX_CALENDAR"]

    cmd = "curl -u %s:%s --upload-file %s %s" % \
          (envDic["LOGIN_ID"],
           envDic["EAS_DEFAULT_PASSWORD"],
           calendarFilePath,
           urlPath)
    print "calendar cmd = %s" % cmd
    rslt = os.popen(cmd)
    for eachLine in rslt:
        print eachLine


@DirOps.decoFun
def executeMsgmv(envDic, withValgrind=False):
    curDir = os.getcwd()
    cmd = "env LD_LIBRARY_PATH=%s " % envDic["LD_LIBRARY_PATH"]
    if withValgrind:
        cmd += envDic["VALGRIND_CMD"]
    cmd += " %s %s %s" \
           % (curDir + "/../bin/msgmv",
              envDic["MSGMV_CONFIG_PATH"],
              envDic["LOGIN_ID"])
    if withValgrind:
        cmd += " 2>>%s" % curDir + "/../log/mv_valgrind.log"
    rslt = os.popen(cmd)
    for eachLine in rslt:
        print eachLine
    print "%s msgmv was executed" % envDic["LOGIN_ID"]


@DirOps.decoFun
def checkMailData(envDic):
    curDir = os.getcwd()
    executePerlScript(curDir + "/listMailData_NSIM.pl", envDic["LOGIN_ID"], "list NSIM data after migrated")


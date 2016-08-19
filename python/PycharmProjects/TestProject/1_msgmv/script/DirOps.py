#!/usr/bin/python
# -*- coding: UTF-8 -*-
import os


def decoFun(func):
    def wrappedFunc(*args, **kwargs):
        print "=====%s() start=====" % func.__name__
        try:
            retval = func(*args, **kwargs)
        except Exception as e:
            print "\nfaild to execute %s: args=%r, kwargs= %r" \
                % (func.__name__, args, kwargs)
            raise e
        print "=====%s() end=======" % func.__name__
        return retval
    return wrappedFunc


@decoFun
def remakeDir(dirPath):
    if os.path.exists(dirPath):
        os.popen("rm -rf %s" % dirPath)
        print "%s was deleted" % dirPath
    os.popen("mkdir -p %s" % dirPath)
    print "%s was made" % dirPath


@decoFun
def iniDir(envDic):
    remakeDir(envDic["TOOLSERV_OUTPUT_DIR"])
    remakeDir(envDic["MAILARCHIVE_UNZIP_DIR"])
    remakeDir(envDic["TOOLSERV_FINISH_DIR"])
    remakeDir(envDic["TOOLSERV_ERR_DIR"])
    remakeDir(envDic["TRANS_PREVENT_DIR"])
    remakeDir(envDic["TOOLSERV_OUTPUT_DIR"] + "/../file/pid")

    logDir = envDic["TOOLSERV_OUTPUT_DIR"] + "/../log"
    if os.path.isdir(logDir):
        for logpath in os.listdir(logDir):
            if logpath == "CaseExe.log":
                continue
            logpath = logDir + "/" + logpath
            if os.path.isfile(logpath):
                logfile = open(logpath, "w")
                logfile.write("")
                logfile.close()
                print "%s was cleared" % logpath


@decoFun
def saveAllEvidence(envDic):
    rootDir = os.getcwd() + "/.."
    confDir = rootDir + "/conf"
    dataDir = rootDir + "/data"
    logDir = rootDir + "/log"
    fileDir = rootDir + "/file"
    scriptDir = rootDir + "/script"
    evdDir = rootDir + "/evidence/%s-%s"
    evdDir = evdDir % (envDic["CASE_NO"], envDic["EXE_TIME2"])
    if not os.path.exists(evdDir):
        os.popen("mkdir -p %s" % evdDir)
    print "evdience folder %s was made" % evdDir

    os.popen("cp %s %s -rf" % (confDir, evdDir))
    print "conf folder was saved under %s" % evdDir

    os.popen("cp %s %s -rf" % (dataDir, evdDir))
    print "data folder was saved under %s" % evdDir

    os.popen("cp %s %s -rf" % (logDir, evdDir))
    print "log folder was saved under %s" % evdDir

    os.popen("cp %s %s -rf" % (fileDir, evdDir))
    print "file folder was saved under %s" % evdDir

    os.popen("cp %s %s -rf" % (scriptDir, evdDir))
    print "script folder was saved under %s" % evdDir

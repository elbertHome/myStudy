#!/usr/bin/python
# -*- coding: UTF-8 -*-
import mysql.connector
import DirOps


@DirOps.decoFun
def getCnx(envDic):
    queConfig = {
        'user': envDic["DB_USER"],
        'password': envDic["DB_PASSWORD"],
        'host': envDic["DB_HOST_NAME"],
        'port': int(envDic["DB_PORT_NO"]),
        'database': envDic["DB_NAME"]}
    planConfig = {
        'user': envDic["PLAN_DB_USER"],
        'password': envDic["PLAN_DB_PASSWORD"],
        'host': envDic["PLAN_DB_HOST_NAME"],
        'port': int(envDic["PLAN_DB_PORT_NO"]),
        'database': envDic["PLAN_DB_NAME"]}
    cnxQue = mysql.connector.connect(**queConfig)
    cnxPlan = mysql.connector.connect(**planConfig)
    return {"que": cnxQue, "plan": cnxPlan}


@DirOps.decoFun
def deleteDBdata(envDic, conns):
    strSql = """delete from %s.%s where LOGIN_ID='%s';""" \
             % (envDic["DB_NAME"],
                envDic["DB_TABLE_NAME"],
                envDic["LOGIN_ID"])
    conns["que"].cursor().execute(strSql)
    conns["que"].commit()
    print "LOGIN_ID=%s was deleted from table %s.%s" \
          % (envDic["LOGIN_ID"], envDic["DB_NAME"], envDic["DB_TABLE_NAME"])

    strSql = """delete from %s.%s where LOGIN_ID='%s';""" \
             % (envDic["PLAN_DB_NAME"],
                envDic["PLAN_DB_TABLE_NAME"],
                envDic["LOGIN_ID"])
    conns["plan"].cursor().execute(strSql)
    conns["plan"].commit()
    print "LOGIN_ID=%s was deleted from table %s.%s" \
          % (envDic["LOGIN_ID"], envDic["PLAN_DB_NAME"], envDic["PLAN_DB_TABLE_NAME"])


@DirOps.decoFun
def insertDBdata(envDic, conns):
    strSql = """insert into %s.%s (
                LOGIN_ID,
                NODE_ID,
                EAS_HOST,
                PROC_STATUS,
                IMAP_STATUS,
                SMTP_STATUS,
                INSERT_TIME)
                values
                ('%s','%s','%s','%s',%d,%d,'%s');""" \
             % (envDic["DB_NAME"],
                envDic["DB_TABLE_NAME"],
                envDic["LOGIN_ID"],
                envDic["NSIM_ID"],
                envDic["MBS_ID"],
                envDic["QUE_PROC_STATUS"],
                envDic["QUE_IMAP_STATUS"],
                envDic["QUE_SMTP_STATUS"],
                envDic["EXE_TIME"])
    conns["que"].cursor().execute(strSql)
    conns["que"].commit()
    print "LOGIN_ID=%s was inserted in table %s.%s" \
          % (envDic["LOGIN_ID"], envDic["DB_NAME"], envDic["DB_TABLE_NAME"])

    strSql = """insert into %s.%s (
                        LOGIN_ID,
                        MAIL_ADDR_EXTRA,
                        MAIL_ADDR,
                        EAS_HOST,
                        NODE_ID,
                        PROC_STATUS,
                        MAIL_MOVE_FLAG,
                        DATA_MOVE_FLAG,
                        OPERATION_FLAG,
                        INSERT_TIME,
                        MDN,
                        SERVICE_ID)
                        values
                        ('%s','%s','%s','%s','%s',%d,%d,%d,%d,'%s','',0);""" \
             % (envDic["PLAN_DB_NAME"],
                envDic["PLAN_DB_TABLE_NAME"],
                envDic["LOGIN_ID"],
                envDic["PLAN_MAIL_ADDR_EXTRA"],
                envDic["PLAN_MAIL_ADDR"],
                envDic["MBS_ID"],
                envDic["NSIM_ID"],
                envDic["PLAN_PROC_STATUS"],
                envDic["PLAN_MAIL_MOVE_FLAG"],
                envDic["PLAN_DATA_MOVE_FLAG"],
                envDic["PLAN_OPERATION_FLAG"],
                envDic["EXE_TIME"])
    conns["plan"].cursor().execute(strSql)
    conns["plan"].commit()
    print "LOGIN_ID=%s was inserted in table %s.%s" \
          % (envDic["LOGIN_ID"], envDic["PLAN_DB_NAME"], envDic["PLAN_DB_TABLE_NAME"])


@DirOps.decoFun
def selectDBdata(envDic, conns):
    lstCol = ["LOGIN_ID",
              "ZIMBRA_ID",
              "NODE_ID",
              "EAS_HOST",
              "PROC_STATUS",
              "IMAP_STATUS",
              "IMAP_RETRY_COUNT",
              "IMAP_SKIP_COUNT",
              "SMTP_MAIL_TOTAL",
              "SMTP_STATUS",
              "RESTART_COUNT",
              "INSERT_TIME",
              "UPDATE_TIME"]
    strSql = """select
                    %s
                from %s.%s where LOGIN_ID='%s';""" \
             % (",".join(lstCol),
                envDic["DB_NAME"],
                envDic["DB_TABLE_NAME"],
                envDic["LOGIN_ID"])
    curs = conns["que"].cursor()
    curs.execute(strSql)
    print "show result in %s.%s" % (envDic["DB_NAME"], envDic["DB_TABLE_NAME"])
    for (i, row) in enumerate(curs.fetchall()):
        print "Row %d in %s.%s" % (i + 1, envDic["DB_NAME"], envDic["DB_TABLE_NAME"])
        for (j, name) in enumerate(lstCol):
            print "\t%s = %s" % (lstCol[j], row[j])
    curs.close()

    lstCol = ["LOGIN_ID",
              "MAIL_ADDR_EXTRA",
              "MAIL_ADDR",
              "MDN",
              "EAS_HOST",
              "NODE_ID",
              "SERVICE_ID",
              "INSERT_TRIGGER",
              "PROC_STATUS",
              "MOVE_TRIGGER",
              "EAS_PROFILE_STATUS",
              "MMS_ACCESS_STATUS",
              "ERROR_NO",
              "BEFORE_NOTICE_NUM",
              "AFTER_NOTICE_NUM",
              "AFTER_NOTICE_STOP",
              "BEFORE_NOTICE_FLAG",
              "AFTER_NOTICE_FLAG",
              "MAIL_MOVE_FLAG",
              "DATA_MOVE_FLAG",
              "PRIORITY",
              "OPERATION_FLAG",
              "MOVE_PLAN_DATE",
              "MOVE_DATE",
              "MOVE_REQUEST_TIME",
              "COMMENT",
              "INSERT_TIME",
              "UPDATE_TIME"]

    strSql = """select
                    %s
                from %s.%s where LOGIN_ID='%s';""" \
             % (",".join(lstCol),
                envDic["PLAN_DB_NAME"],
                envDic["PLAN_DB_TABLE_NAME"],
                envDic["LOGIN_ID"])
    curs = conns["plan"].cursor()
    curs.execute(strSql)
    print "show result in %s.%s" \
          % (envDic["PLAN_DB_NAME"], envDic["PLAN_DB_TABLE_NAME"])
    print ""
    for (i, row) in enumerate(curs.fetchall()):
        print "Row %d in %s.%s" \
              % (i + 1, envDic["PLAN_DB_NAME"], envDic["PLAN_DB_TABLE_NAME"])
        for (j, name) in enumerate(lstCol):
            print "\t%s = %s" % (lstCol[j], row[j])
    curs.close()


@DirOps.decoFun
def prepareDBdata(envDic):
    conns = getCnx(envDic)
    deleteDBdata(envDic, conns)
    insertDBdata(envDic, conns)
    selectDBdata(envDic, conns)

    conns["que"].close()
    conns["plan"].close()


@DirOps.decoFun
def checkDBData(envDic):
    conns = getCnx(envDic)
    selectDBdata(envDic, conns)




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
    cnxQue = mysql.connector.connect(**queConfig)
    return cnxQue


@DirOps.decoFun
def deleteDBdata(envDic, conn):
    strSql = """delete from %s.%s where LOGIN_ID='%s';""" \
             % (envDic["DB_NAME"],
                envDic["DB_TABLE_NAME"],
                envDic["LOGIN_ID"])
    conn.cursor().execute(strSql)
    conn.commit()
    print "LOGIN_ID=%s was deleted from table %s.%s" \
          % (envDic["LOGIN_ID"], envDic["DB_NAME"], envDic["DB_TABLE_NAME"])


@DirOps.decoFun
def insertDBdata(envDic, conn):
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
    conn.cursor().execute(strSql)
    conn.commit()
    print "LOGIN_ID=%s was inserted in table %s.%s" \
          % (envDic["LOGIN_ID"], envDic["DB_NAME"], envDic["DB_TABLE_NAME"])


@DirOps.decoFun
def selectDBdata(envDic, conn):
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
    curs = conn.cursor()
    curs.execute(strSql)
    print "show result in %s.%s" % (envDic["DB_NAME"], envDic["DB_TABLE_NAME"])
    for (i, row) in enumerate(curs.fetchall()):
        print "Row %d in %s.%s" % (i + 1, envDic["DB_NAME"], envDic["DB_TABLE_NAME"])
        for (j, name) in enumerate(lstCol):
            print "\t%s = %s" % (lstCol[j], row[j])
    curs.close()


@DirOps.decoFun
def prepareDBdata(envDic):
    conn = getCnx(envDic)
    deleteDBdata(envDic, conn)
    insertDBdata(envDic, conn)
    selectDBdata(envDic, conn)
    conn.close()


@DirOps.decoFun
def checkDBData(envDic):
    conn = getCnx(envDic)
    selectDBdata(envDic, conn)




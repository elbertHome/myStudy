#!/usr/bin/python
# -*- coding: UTF-8 -*-

import sys
import hmac
import hashlib
import mysql.connector
from SimpleHTTPServer import SimpleHTTPRequestHandler
from BaseHTTPServer import HTTPServer


envDic = dict()


def loadConfig(configPath):
    fp = open(configPath, "r")
    for eachline in fp:
        eachline = eachline.strip()
        if eachline.startswith("#") \
                or len(eachline) == 0:
            continue
        spltList = eachline.split("=")
        key = spltList[0].strip()
        val = "".join(spltList[1:]).strip()
        envDic[str(key)] = str(val)
    fp.close()
    print "%s was loaded" % configPath


def getCnx():
    configs = {
        'user': envDic["DB_USER"],
        'password': envDic["DB_PASSWORD"],
        'host': envDic["DB_HOST"],
        'port': int(envDic["DB_PORT"]),
        'database': envDic["DB_NAME"]}

    return mysql.connector.connect(**configs)


def authUser(strAuthUser, conns):
    lstCol = ["user_name",
              "user_pwd",
              "user_login_id",
              "fixed_login_pwd",
              "imap_serv_ip"]

    strSql = """select %s from %s.%s where user_name='%s'""" \
             % (",".join(lstCol),
                envDic["DB_NAME"],
                envDic["TABLE_NAME"],
                strAuthUser)
    curs = conns.cursor()
    curs.execute(strSql)
    rslt = curs.fetchone()
    curs.close()
    if rslt is None:
        return None
    else:
        return dict(zip(tuple(lstCol), rslt))


def getEncWithHmacMD5(enKey, enStr):
    return hmac.new(enKey, enStr, hashlib.md5).hexdigest()


class MyHandler(SimpleHTTPRequestHandler):
    def do_GET(self):
        try:
            print "get request headers"
            for key, val in self.headers.items():
                if isinstance(val, basestring):
                    print "%s = %s" % (key, val)

            conns = getCnx()
            rsltDic = authUser(self.headers.get("Auth-User"), conns)
            intUserAuthenticate = 0
            curPassword = ""
            strAuthServ = ""
            strAuthPort = 0
            if rsltDic is not None:
                if self.headers.get("Auth-Protocol") == "imap":
                    strAuthServ = rsltDic["imap_serv_ip"]
                    strAuthPort = int(envDic["EMAIL_MX_IMAP_PORT"])
                    if self.headers.get("Auth-Method") == "plain":
                        curPassword = rsltDic["user_pwd"]
                    elif self.headers.get("Auth-Method") == "cram-md5":
                        curPassword = getEncWithHmacMD5(str(rsltDic["user_pwd"]), self.headers.get("Auth-Salt"))

                    if curPassword == self.headers.get("Auth-Pass"):
                        intUserAuthenticate = 1
                    else:
                        intUserAuthenticate = 0
                elif self.headers.get("Auth-Protocol") == "smtp":
                    strAuthServ = "15.210.144.217"
                    strAuthPort = 25
                    if self.headers.get("Auth-Method") == "plain":
                        curPassword = rsltDic["user_pwd"]
                    if curPassword == self.headers.get("Auth-Pass"):
                        intUserAuthenticate = 1
                    else:
                        intUserAuthenticate = 0
                else:
                    intUserAuthenticate = -1

            if intUserAuthenticate == -1:   # protocol is wrong
                self.send_response(200)
                self.send_header("Auth-Status", "Invalid protocol")
                #self.send_header("Auth-Wait", 3)
                self.end_headers()
                self.wfile.write("Failure")
                print "Invalid protocol"
            elif intUserAuthenticate == 0:  # username or password is wrong
                self.send_response(200)
                self.send_header("Auth-Status", "Invalid login or password from http-auth-serv")
                #self.send_header("Auth-Wait", 3)
                self.end_headers()
                self.wfile.write("Failure")
                print "Invalid login or password"
            elif intUserAuthenticate == 1:  # username or password is OK
                self.send_response(200)
                self.send_header("Auth-Status", "OK")
                self.send_header("Auth-User", rsltDic["user_login_id"])
                self.send_header("Auth-Pass", rsltDic["fixed_login_pwd"])
                self.send_header("Auth-Server", strAuthServ)
                self.send_header("Auth-Port", strAuthPort)
                self.end_headers()
                self.wfile.write("SUCCESS")
                print "OK"

        except IOError as e:
            message = "File not found: %s\nError:%s" % (self.path, e)
            self.send_response(404, message)


def main():

    if len(sys.argv) <= 1:
        print "useage: mailAuth4Nginx.py [Configuration]"
    else:
        loadConfig(sys.argv[1])
        print envDic
        server = HTTPServer(("", int(envDic["HTTP_LISTEN_PORT"])), MyHandler)
        print "welcome to the marchine at port %s" % envDic["HTTP_LISTEN_PORT"]
        print "Press ^C once or twice to quit"
        try:
            server.serve_forever()
        except KeyboardInterrupt:
            print "^C received, shutting down server"
            server.socket.close()


if __name__ == "__main__":
    main()

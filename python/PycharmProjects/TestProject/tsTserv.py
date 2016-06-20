from socket import *
from time import ctime

HOST = ''
PORT = 21567
BUFSIZ = 1024
ADDR = (HOST, PORT)

tcpSerSock = socket(AF_INET, SOCK_STREAM)
tcpSerSock.settimeout(300)
tcpSerSock.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
tcpSerSock.bind(ADDR)
tcpSerSock.listen(5)

while True:
    print "waiting for connection"
    tcpCliSock, cliaddr = tcpSerSock.accept()
    tcpCliSock.send("welcome to connect TimeServ at %s" % str(cliaddr))
    print "...connected from:", cliaddr

    while True:
        data = tcpCliSock.recv(BUFSIZ)
        data = data.strip()
        if not data:
            break
        tcpCliSock.send("[%s] %s" % (ctime(), data))

    tcpCliSock.close()
tcpSerSock.close()



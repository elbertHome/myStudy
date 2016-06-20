from socket import *

SRVHOST = 'localhost'
SRVPORT = 21567
BUFSIZ = 1024
ADDR = (SRVHOST, SRVPORT)

tcpCliSock = socket(AF_INET, SOCK_STREAM)
tcpCliSock.connect(ADDR)
print tcpCliSock.recv(BUFSIZ)
while True:
    data = raw_input('> ')
    if not data:
        break
    tcpCliSock.send(data)
    data = tcpCliSock.recv(BUFSIZ)
    if not data:
        break
    print data
tcpCliSock.close()

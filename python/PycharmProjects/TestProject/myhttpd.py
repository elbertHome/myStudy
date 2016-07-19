#!/usr/local/bin/python
# -*- coding: UTF-8 -*-


from SimpleHTTPServer import SimpleHTTPRequestHandler
from BaseHTTPServer import HTTPServer


class MyHandler(SimpleHTTPRequestHandler):
    def do_GET(self):
        try:
            f = open(self.path[1:], "r")
            self.send_response(200)
            self.send_header("Content-type", "text/plain")
            self.end_headers()
            self.wfile.write(f.read())
            f.close()
        except IOError as e:
            message = "File not found: %s\nError:%s" % (self.path, e)
            self.send_response(404, message)


def main():
    try:
        server = HTTPServer(("", 8000), MyHandler)
        print "welcome to the marchine..."
        print "Press ^C once or twice to quit"
        # when https use below
        # import ssl
        # import os
        # CERTIFICATE_PATH = os.getcwd() + '/serverCA/server/server-cert.pem'
        # KEY_PATH = os.getcwd() + '/serverCA/server/server-key.pem'
        # server.socket = ssl.wrap_socket(server.socket, keyfile=KEY_PATH, certfile=CERTIFICATE_PATH, server_side=True)
        server.serve_forever()
    except KeyboardInterrupt:
        print "^C received, shutting down server"
        server.socket.close()


if __name__ == "__main__":
    main()

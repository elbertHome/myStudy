#!/usr/local/bin/python
# -*- coding: UTF-8 -*-

import cgi

reshtml = """Content-Type: text/html\n
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
    "http://www.w3.org/TR/html4/loose.dtd">
<html>
    <head>
        <title>
            Friends CGI Demo (dynamic screen)
        </title>
    </head>
    <body>
        <h3>
            Friends list for:
            <i>%s</i>
        </h3>
        your name is: <b>%s</b>
        <p>
        you have <b>%s</b> friends.
    </body>
</html>
"""

form = cgi.FieldStorage()
who = form["person"].value
howmany = form["howmany"].value
print reshtml % (who, who, howmany)

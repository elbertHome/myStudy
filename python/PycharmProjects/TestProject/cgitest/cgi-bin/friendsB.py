#!/usr/local/bin/python
# -*- coding: UTF-8 -*-

import cgi

header = "Content-Type: text/html\n\n"

formhtml = """
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
    "http://www.w3.org/TR/html4/loose.dtd">
<html>
    <head>
        <title>
            Friends CGI Demo
        </title>
    </head>
    <body>
        <h3>
            Friends list for:
            <i>NEW USER</i>
        </h3>
        <form action="/cgi-bin/friendsB.py">
            <b>
                Enter your name:
            </b>
            <input type="hidden" name="action" value="edit">
            <input type="text" name="person" value="NEW USER" size="15">
            <p>
            <b>
                How many friends do you have?
            </b>
            %s
            <p>
            <input type="submit">
        </form>
    </body>
</html>
"""

fradio = """<input type="radio" name=howmany value="%s" %s> %s \n"""

reshtml = """
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


def showForm():
    friends = []
    for i in (0, 10, 25, 50, 100):
        checked = ""
        if i == 0:
            checked = "checked"
        friends.append(fradio % (str(i), checked, str(i)))

    print "%s%s" % (header, formhtml % "".join(friends))


def doResults(who, howmany):
    print header + reshtml % (who, who, howmany)


def process():
    form = cgi.FieldStorage()
    if "person" in form:
        who = form["person"].value
    else:
        who = "New USER"

    if "howmany" in form:
        howmany = form["howmany"].value
    else:
        howmany = 0

    if "action" in form:
        doResults(who, howmany)
    else:
        showForm()


if __name__ == "__main__":
    process()

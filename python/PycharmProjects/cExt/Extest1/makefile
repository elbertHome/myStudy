CC=gcc
CFLAGS=-Wall -g -I/usr/local/python2.7.11/include/python2.7
OBJS=Extest1.o
DEPS=

all: Extest

Extest: Extest1.c
	$(CC) $(CFLAGS) Extest1.c -o Extest

clean:
	rm -f *.o
	rm -f Extest

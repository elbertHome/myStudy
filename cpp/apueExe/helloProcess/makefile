CC=gcc
CFLAGS=-Wall -g -I/usr/local/include/apue3
LDFLAGS=-lapue
PROGRAM=hellopid

 all: $(PROGRAM)

$(PROGRAM): hellopid.c
	$(CC) $(CFLAGS) -o $(PROGRAM) hellopid.c $(LIB)

clean:
	rm -f *.o
	rm -f $(PROGRAM)

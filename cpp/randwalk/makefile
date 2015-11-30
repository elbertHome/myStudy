CC=g++
CFLAGS= -g -O0 -Wall 
OBJS=vector.o
DEPS=vector.hpp

PROGRAM=randwalk

all:$(PROGRAM)

$(PROGRAM): randwalk.cpp $(DEPS) $(OBJS)
	$(CC) $(CFLAGS) randwalk.cpp $(OBJS) -o $@

vector.o: vector.cpp $(DEPS)
	$(CC) $(CFLAGS) vector.cpp -c

clean:
	rm -f *.o
	rm -f $(PROGRAM)

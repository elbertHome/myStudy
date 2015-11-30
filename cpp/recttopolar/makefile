CC=g++
CFLAGS= -g -O0 -Wall 
OBJS=coordin.o
DEPS=coordin.hpp

all:rect_polar

rect_polar: main.cpp $(DEPS) $(OBJS)
	$(CC) $(CFLAGS) main.cpp $(OBJS) -o rect_polar

coordin.o: coordin.cpp $(DEPS)
	$(CC) $(CFLAGS) coordin.cpp -c

clean:
	rm -f *.o
	rm -f rect_polar

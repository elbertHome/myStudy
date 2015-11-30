CC=g++
CFLAGS=-g -Wall
LIB=-lpthread
OBJS=Configuration.o commontools.o
DEPS=Configuration.hpp commontools.hpp
PROGRAM=sim_nsim

all: $(PROGRAM) 

$(PROGRAM): sim_nsim_main.cpp $(DEPS) $(OBJS)
	$(CC) -o $(PROGRAM) $(CFLAGS) sim_nsim_main.cpp $(OBJS) $(LIB)

Configuration.o: Configuration.cpp $(DEPS)
	$(CC) $(CFLAGS) Configuration.cpp -c

commontools.o: commontools.cpp $(DEPS)
	$(CC) $(CFLAGS) commontools.cpp -c

clean:
	rm -f *.o
	rm -f $(PROGRAM)

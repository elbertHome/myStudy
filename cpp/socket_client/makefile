CC=g++
CFLAGS=-g -Wall
CLIENT=socketclient

all: $(CLIENT)

$(CLIENT): socketclient.cpp
	$(CC) -o $(CLIENT) $(CFLAGS) socketclient.cpp 

clean:
	rm -f *.o
	rm -f $(CLIENT)

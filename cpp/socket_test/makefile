CC=g++
CFLAGS=-g -Wall
LIB=-lpthread
SERVER=server_test
CLIENT=client_test

all: $(SERVER) $(CLIENT)

$(SERVER): server_test.cpp 
	$(CC) -o $(SERVER) $(CFLAGS) server_test.cpp $(LIB)

$(CLIENT): client_test.cpp
	$(CC) -o $(CLIENT) $(CFLAGS) client_test.cpp 

clean:
	rm -f *.o
	rm -f $(SERVER)
	rm -f $(CLIENT)

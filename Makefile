CC=g++ -std=c++11 -Wall

all: client server

client: client.cc socket.cc protocol.h
	$(CC) socket.cc client.cc -o client

server: server.cc socket.cc protocol.h
	$(CC) socket.cc server.cc -lpthread -o server

clean:
	rm -f client server

.PHONY: all clean

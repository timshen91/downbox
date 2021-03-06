CC=g++ -std=c++11 -Wall

all: client server

client: client.cc *.h
	$(CC) client.cc -o client

server: server.cc *.h
	$(CC) server.cc -lpthread -o server

clean:
	rm -f client server

.PHONY: all clean

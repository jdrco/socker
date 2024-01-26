CC=gcc
CFLAGS=-Wall -Wextra -g

all: client server

client: client.o util.o
	$(CC) $(CFLAGS) -o client client.o util.o

server: server.o util.o
	$(CC) $(CFLAGS) -o server server.o util.o

client.o: client.c util.h
	$(CC) $(CFLAGS) -c client.c

server.o: server.c util.h
	$(CC) $(CFLAGS) -c server.c

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c

clean:
	rm -f client server *.o

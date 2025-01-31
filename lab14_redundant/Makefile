CFLAGS := -g -O0 -Wall -pthread

all: client server

server: server.c
	gcc -o server $(CFLAGS) server.c

client: client.c
	gcc -o client $(CFLAGS) client.c

clean:
	rm client server

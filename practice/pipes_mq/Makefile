CFLAGS:= -g -O0 -Wall -lpthread

all: server

server: server.c
	gcc -o server $(CFLAGS) server.c
clean:
	rm server
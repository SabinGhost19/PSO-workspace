CFLAGS:= -g -O0 -Wall -pthread

all: server file prr

server: server.c
	gcc -o server $(CFLAGS) server.c
file: file.c
	gcc -o file $(CFLAGS) file.c
prr: prr.c
	gcc -o prr $(CFLAGS) prr.c
clean:
	rm server file prr
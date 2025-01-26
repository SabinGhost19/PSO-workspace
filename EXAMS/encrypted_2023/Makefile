CFLAGS := -O0 -Wall -lpthread -lssl -lcrypto
all: server

server: server.c
	gcc -o server $(CFLAGS) server.c
clean:
	rm  server

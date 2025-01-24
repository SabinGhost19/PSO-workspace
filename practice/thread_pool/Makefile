CFLAGS := -g -O0 -Wall -pthread

all: client server pool

server: server.c
	gcc -o server $(CFLAGS) server.c

client: client.c
	gcc -o client $(CFLAGS) client.c

pool: pool.c
	gcc -o pool $(CFLAGS) pool.c

clean:
	rm client server pool

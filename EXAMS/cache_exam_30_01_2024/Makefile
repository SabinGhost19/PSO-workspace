CFLAGS := -g -O0 -Wall -lpthread
all: server client
server: server.c
	gcc -o server $(CFLAGS) server.c
client: client.c
	gcc -o client $(CFLAGS) client.c
clean:
	rm server client /dev/mqueue/c* /dev/shm/C*
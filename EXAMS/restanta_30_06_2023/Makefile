CFLAGS := -O0 -Wall -lpthread
all: server server_semaphore

server: server.c
	gcc -o server $(CFLAGS) server.c

server_semaphore: server_semaphore.c
	gcc -o server_semaphore $(CFLAGS) server_semaphore.c

clean:
	rm  server server_semaphore

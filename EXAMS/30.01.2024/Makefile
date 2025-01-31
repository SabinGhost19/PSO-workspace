FLAGS = -Wall -g

all: server client

server: server.c utils.c utils.h
	gcc $(FLAGS) server.c -o server 

client: client.c utils.c utils.h
	gcc $(FLAGS) client.c -o client
clean:
	rm server client

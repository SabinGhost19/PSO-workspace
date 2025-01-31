#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "utils.h"

#define MAX_CACHE_SLOTS 10
#define MAX_CACHE_BYTES 100
#define CACHE_FILENAME "cache_file.txt"

struct cache
{
    char filename[1024];
    char bytes[MAX_CACHE_BYTES];
    size_t byte_start;
    int used_nr;
    short is_used;
};

typedef struct cache cache_t;

pthread_mutex_t cache_mutex;
void *cache_mmap = NULL;

void init_data()
{
    pthread_mutex_init(&cache_mutex, NULL);
    int fd = shm_open("name", O_RDWR | O_CREAT, 0664);
    ftruncate(fd, MAX_CACHE_SLOTS * sizeof(cache_t));
    cache_mmap = mmap(NULL, MAX_CACHE_SLOTS * sizeof(cache_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, fd, 0);
}

void clear_data()
{
}

void send_data(char *filename, int byte_start, int bytes_size, int client_sock)
{
    int fd = open(filename, O_RDONLY);
    lseek(fd, byte_start, SEEK_SET);
    char *buffer = (char *)malloc(bytes_size);
    read(fd, buffer, bytes_size);
    send(client_sock, buffer, bytes_size, 0);
    close(fd);
}

void store_cache(char *filename, int byte_start, int bytes_size)
{
    int least_used = 999;
    int least_used_index = -1;
    pthread_mutex_lock(&cache_mutex);
    while (bytes_size > 0)
    {
        for (int i = 0; i < MAX_CACHE_SLOTS && bytes_size > 0; i++) // check if all is used
        {
            cache_t *ch = (cache_t *)(cache_mmap + i * sizeof(cache_t));
            if (!ch->is_used)
            {
                ch->is_used = 1;
                strcpy(ch->filename, filename);
                int start = (byte_start / 100) * 100;
                ch->byte_start = start;
                int fd = open(filename, O_RDONLY);
                lseek(fd, ch->byte_start, SEEK_SET);
                read(fd, ch->bytes, MAX_CACHE_BYTES);
                close(fd);
                printf("Storing cache for %s\n", ch->bytes);

                int size = bytes_size % MAX_CACHE_BYTES;
                bytes_size = bytes_size - size;
                if (byte_start + bytes_size > ch->byte_start + MAX_CACHE_BYTES)
                {
                    byte_start = ch->byte_start + MAX_CACHE_BYTES + 1;
                    i = 0;
                    continue;
                }
            }
            else
            {
                if (ch->used_nr < least_used)
                {
                    least_used = ch->used_nr;
                    least_used_index = i;
                }
            }
        }
        if (bytes_size > 0)
        {
            cache_t *ch = (cache_t *)(cache_mmap + least_used_index * sizeof(cache_t));
            strcpy(ch->filename, filename);
            int start = (byte_start / 100) * 100;
            ch->byte_start = start;
            int fd = open(filename, O_RDONLY);
            read(fd, ch->bytes, MAX_CACHE_BYTES);
            close(fd);
            int size = ch->byte_start - byte_start + bytes_size;
            bytes_size = bytes_size - size;
            msync(cache_mmap, MAX_CACHE_SLOTS * sizeof(cache_t), 0);
            if (byte_start + bytes_size > ch->byte_start + MAX_CACHE_BYTES)
            {
                byte_start = ch->byte_start + MAX_CACHE_BYTES + 1;
                continue;
            }
        }
    }
    pthread_mutex_unlock(&cache_mutex);
}

void get_from_cache(char *filename, int byte_start, int bytes_size, int client_sock)
{
    pthread_mutex_lock(&cache_mutex);
    int index = 0;
    char buffer[MAX_CACHE_BYTES] = "";
    int size;
    int found = 0;
    while (!found)
    {
        while (index < MAX_CACHE_SLOTS && found == 0)
        {
            cache_t *ch = (cache_t *)(cache_mmap + index * sizeof(cache_t));
            if (ch->is_used && strcmp(ch->filename, filename) == 0 && byte_start >= (ch->byte_start) && byte_start < (ch->byte_start + MAX_CACHE_BYTES))
            {
                size = bytes_size % MAX_CACHE_BYTES;
                strcpy(buffer, ch->bytes + (byte_start - ch->byte_start));
                buffer[size] = '\0';
                strcpy(buffer + size, "");

#ifdef DEBUG
                printf("Accessing from cache!\n");
#endif
                send(client_sock, buffer, size, 0);
                found = 1;
                if (byte_start + bytes_size > ch->byte_start + MAX_CACHE_BYTES)
                {
                    found = 0;
                    byte_start = ch->byte_start + MAX_CACHE_BYTES + 1;
                    bytes_size = bytes_size - size;
                    index = 0;
                    continue;
                }
            }
            index++;
        }
        pthread_mutex_unlock(&cache_mutex);
        if (!found)
        {
            store_cache(filename, byte_start, bytes_size);
            index = 0;
        }
    }
}

void write_to_file(cache_t *ch)
{
#ifdef DEBUG
    printf("Writing to file: %s\n", ch->bytes);
#endif
    int fd = open(ch->filename, O_WRONLY);
    lseek(fd, ch->byte_start, SEEK_SET);
    write(fd, ch->bytes, MAX_CACHE_BYTES);
    close(fd);
}

void write_to_cache(char *filename, int byte_start, int bytes_size, int client_sock, char *bytes)
{
    pthread_mutex_lock(&cache_mutex);
    int index = 0;
    char buffer[MAX_CACHE_BYTES] = "";
    int size;
    int found = 0;
    while (!found)
    {
        while (index < MAX_CACHE_SLOTS && found == 0)
        {
            cache_t *ch = (cache_t *)(cache_mmap + index * sizeof(cache_t));
            if (ch->is_used && strcmp(ch->filename, filename) == 0 && byte_start >= (ch->byte_start) && byte_start < (ch->byte_start + MAX_CACHE_BYTES))
            {
                size = bytes_size % MAX_CACHE_BYTES;
                buffer[size] = '\0';
#ifdef DEBUG
                printf("Accessing form cache!(writing)\n");
#endif
                found = 1;
                strncpy(buffer, bytes, size);
                char saved[MAX_CACHE_BYTES] = "";
                strcpy(saved, ch->bytes);
                saved[byte_start] = '\0';
                strcpy(ch->bytes, ch->bytes + size);
                strcat(saved, buffer);
                strcat(saved, ch->bytes);
                strcpy(ch->bytes, saved);
                write_to_file(ch);
                strcpy(bytes, bytes + size);
                if (byte_start + bytes_size > ch->byte_start + MAX_CACHE_BYTES)
                {
                    found = 0;
                    byte_start = ch->byte_start + MAX_CACHE_BYTES + 1;
                    bytes_size = bytes_size - size;
                    index = 0;
                    continue;
                }
            }
            index++;
        }
        pthread_mutex_unlock(&cache_mutex);
        if (!found)
        {
            store_cache(filename, byte_start, bytes_size);
            index = 0;
        }
    }
}

void *handle_client(void *args)
{
    int client_socket = *(int *)args;
    message_container_t msg;
    int fd;
    while (recv(client_socket, &msg, sizeof(message_container_t), 0) > 0)
    {
#ifdef DEBUG
        printf("recieved! filename: %s\n", msg.filename);
        fflush(0);
#endif
        if (msg.op == 'w')
        {
            fd = open(msg.filename, O_WRONLY | O_CREAT, 0664);
            lseek(fd, msg.start_byte, SEEK_SET);
            char *buffer = (char *)malloc(msg.size_bytes);
            recv(client_socket, buffer, sizeof(buffer), 0);

            close(fd);
            write_to_cache(msg.filename, msg.start_byte, msg.size_bytes, client_socket, buffer);
        }
        else if (msg.op == 'r')
        {
            get_from_cache(msg.filename, msg.start_byte, msg.size_bytes, client_socket);
        }
    }
    printf("Client Disconnected.\n");
    return NULL;
}

int main(void)
{
    int socket_desc, client_sock;
    socklen_t client_size;
    struct sockaddr_in server_addr, client_addr;

    // Create socket:
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0)
    {
        printf("Error while creating socket\n");
        return -1;
    }

    // Set port and IP that we'll be listening for, any other IP_SRC or port will be dropped:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind to the set port and IP:
    if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Couldn't bind to the port\n");
        return -1;
    }

    // Listen for clients:
    if (listen(socket_desc, 1) < 0)
    {
        printf("Error while listening\n");
        return -1;
    }
    printf("\nListening for incoming connections.....\n");

    init_data();

    pthread_t tid;
    while (1)
    {
        // Accept an incoming connection from one of the clients:
        client_size = sizeof(client_addr);
        client_sock = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);

        if (client_sock < 0)
        {
            printf("Can't accept\n");
            return -1;
        }
        printf("Client connected at IP: %s and port: %i for socket: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_sock);
        if (pthread_create(&tid, NULL, handle_client, &client_sock))
        {
            perror("Error creating thread.\n");
            exit(1);
        }
    }

    // Closing the socket:
    close(client_sock);
    close(socket_desc);

    clear_data();

    return 0;
}
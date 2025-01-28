#define _GNU_SOURCE
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <sys/sendfile.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#define PORT 2341
#define NUM_THREADS 50000
#define BUF_SIZE 1024
int socket_desc;
int finished = 0;
pthread_mutex_t finished_mutex = PTHREAD_MUTEX_INITIALIZER;
struct sockaddr_in server_addr, client_addr;
void init_connection();

void *conn_handler_by_pthread(void *args)
{
    char buffer[BUF_SIZE];
    int client_sock = *((int *)args);
    printf("COnn socket client: %d\n", client_sock);

    if (recv(client_sock, &buffer, sizeof(buffer), 0) < 0)
    {
        printf("Couldn't receive\n");
        exit(-1);
    }
    printf("Msg from client: %s\n", buffer);

    // Respond to client:
    // char server_message[BUF_SIZE];
    // strcpy(server_message, "RASPUNS");
    // if (send(client_sock, server_message, strlen(server_message), 0) < 0)
    // {
    //     printf("Can't send\n");
    //     exit(-1);
    // }

    // buffer[strcspn(buffer, ",")] = '\0';
    // printf("RESSSS:%s...\n", buffer);
    char server_message[BUF_SIZE];
    char op_code[BUF_SIZE], filename[BUF_SIZE], file_size[BUF_SIZE];
    sscanf(buffer, "%[^,],%[^,],%[^\n]", op_code, filename, file_size);
    printf("%s...%s...%s...\n", op_code, filename, file_size);

    if (strcmp(op_code, "DOWN") == 0)
    {
        int fd = open(filename, O_RDWR);
        if (fd < 0)
        {
            perror("error opening file");
            exit(EXIT_FAILURE);
        }
        struct stat file_stat;
        int stat = 0;
        fstat(fd, &file_stat);
        sendfile(client_sock, fd, 0, file_stat.st_size);
        printf("Trimis....\n");
    }
    else
    {
        // Respond to client : char server_message[BUF_SIZE];
        strcpy(server_message, "RASPUNS");
        if (send(client_sock, server_message, strlen(server_message), 0) < 0)
        {
            printf("Can't send\n");
            exit(-1);
        }
    }
    close(client_sock);
    return NULL;
}
void handle_signal(int sig)
{
    pthread_mutex_lock(&finished_mutex);
    finished = 1;
    pthread_mutex_unlock(&finished_mutex);
}
int main(void)
{
    struct sigaction sa = {0};
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = &handle_signal;
    if (sigaction(SIGUSR2, &sa, NULL) == -1)
    {
        perror("sigaction failed");
        exit(1);
    }

    init_connection();
    int client_sock[NUM_THREADS];
    int current = 0;
    int client_size = sizeof(client_addr);
    pthread_t tid[NUM_THREADS];

    pthread_mutex_lock(&finished_mutex);
    printf("%d\n", getpid());
    int retval = 0;
    fd_set rfds;
    struct timeval tv;
    while (finished != 1)
    {
        pthread_mutex_unlock(&finished_mutex);

        FD_ZERO(&rfds);
        FD_SET(socket_desc, &rfds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        retval = select(socket_desc + 1, &rfds, NULL, NULL, &tv);
        if (retval == -1)
        {
            if (errno == EINTR)
            {
                pthread_mutex_lock(&finished_mutex);
                continue;
            }

            perror("Eroare la select!\n");
            exit(EXIT_FAILURE);
        }
        else if (retval)
        {
            printf("Data is available now.\n");
            client_sock[current] = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);
            if (client_sock[current] < 0)
            {
                if (errno == EINTR)
                    continue;
                printf("Can't accept\n");
                return -1;
            }
            printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            pthread_create(&tid[current], NULL, conn_handler_by_pthread, (void *)&client_sock[current]);
            current++;
        }
        pthread_mutex_lock(&finished_mutex);
    }

    for (int i = 0; i < current; i++)
    {
        pthread_join(tid[i], NULL);
    }
    close(socket_desc);
    printf("END...\n");

    return 0;
}
void init_connection()
{
    // Create socket:
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0)
    {
        printf("Error while creating socket\n");
        exit(1);
    }
    printf("Socket created successfully\n");

    // Set port and IP that we'll be listening for, any other IP_SRC or port will be dropped:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind to the set port and IP:
    if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Couldn't bind to the port\n");
        exit(1);
    }
    printf("Done with binding\n");

    // Listen for clients:
    if (listen(socket_desc, 1) < 0)
    {
        printf("Error while listening\n");
        exit(1);
    }
    printf("\nListening for incoming connections.....\n");
}

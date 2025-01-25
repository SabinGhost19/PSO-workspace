#define _GNU_SOURCE
#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#define BUF_SIZE (1 << 13)
#define NAME "/test_queue"

// varianta complexa
// void signal_handler(int signal, siginfo_t *info void *p)
// {
// }

// varianta simpla:
void signal_handler(int signal)
{
}
void set_signal()
{

    // zeroizam neaparat
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    printf("Process %d started.\n", getpid());

    // fara SA_RESETHAND
    // sa.sa_flags = SA_RESETHAND;

    sa.sa_handler = signal_handler;
    sigaction(SIGTERM, &sa, NULL);
}

char buf[BUF_SIZE];

int id_seed = 123456;
void *thread_handler(void *params)
{
    char *message = params;
    int client = *((int *)params);
    int id = id_seed++;

    // facem recv pe server aici

    return NULL;
}

//!!!!!!!!!! TOT CE FAC E CU STRINGURI
// PEMNTRU CA STIU CA SUNT STRIGURI SI LUCREZ CU ELE
int main(int argc, char **argv)
{
    int socket_desc, client_sock, client_size;
    struct sockaddr_in server_addr, client_addr;
    char server_message[2000], client_message[2000];

    // Clean buffers:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));

    // Create socket:
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0)
    {
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");

    // Set port and IP that we'll be listening for, any other IP_SRC or port will be dropped:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind to the set port and IP:
    if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Couldn't bind to the port\n");
        return -1;
    }
    printf("Done with binding\n");

    // Listen for clients:
    if (listen(socket_desc, 1) < 0)
    {
        printf("Error while listening\n");
        return -1;
    }
    printf("\nListening for incoming connections.....\n");

    // face copie la buffer pentru a nu fi suprascris
    // TODO: sincronize thread resources
    pthread_t tid[5];
    char *param[5];
    for (int i = 0; i < 5; i++)
    {
        param[i] = malloc(BUF_SIZE);
    }

    int current = 0;
    while (1)
    {

        strcpy(param[current], buf);
        client_size = sizeof(client_addr);
        client_sock = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);

        if (client_sock < 0)
        {
            printf("Can't accept\n");
            return -1;
        }
        printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        pthread_create(&tid[2], NULL, thread_handler, param[current]);
    }
    // // nu ma intereseaza prioritatea deocamdata
    // // NULL, acolo defapt e un pointer la int
    // // unde este prioritatea cu care e venit mesajul
    // mq_receive(m, buf, BUF_SIZE, NULL);

    printf("Received: %s\n", buf);

    return 0;
}
#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#define BUF_SIZE (1 << 13)
#define TEXT "test message"
#define NAME "/test_queue"

char buf[BUF_SIZE];

int main(int argc, char **argv)
{
    int socket_desc;
    struct sockaddr_in server_addr;
    char server_message[2000], client_message[2000];

    // Clean buffers:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));

    // Create socket, we use SOCK_STREAM for TCP
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0)
    {
        printf("Unable to create socket\n");
        return -1;
    }

    printf("Socket created successfully\n");

    // Set port and IP the same as server-side:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Send connection request to server:
    if (connect(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Unable to connect\n");
        return -1;
    }
    printf("Connected with server successfully\n");

    // Get input from the user:
    printf("Enter message: ");
    gets(client_message);

    // Send the message to server:
    if (send(socket_desc, client_message, strlen(client_message), 0) < 0)
    {
        printf("Unable to send message\n");
        return -1;
    }

    // // Receive the server's response:
    // if (recv(socket_desc, server_message, sizeof(server_message), 0) < 0)
    // {
    //     printf("Error while receiving server's msg\n");
    //     return -1;
    // }

    // printf("Server's response: %s\n", server_message);

    // Close the socket:
    close(socket_desc);

    return 0;
}

// int main(int argc, char **argv)
// {
//     unsigned int prio = 10;
//     mqd_t m;
//     if (argc < 2)
//     {
//         printf("Command not specified!!.....\n");
//         exit(-1);
//     }

//     // clientul pleaca de la premisa ca exista coada
//     m = mq_open(NAME, O_RDWR, 0666, NULL);
//     if (m == -1)
//     {
//         perror("mq_open");
//         exit(-1);
//     }

//     // luam comanda si o punem in buffer
//     strcpy(buf, argv[1]);

//     // cu priority 10 ...idk
//     mq_send(m, buf, strlen(buf), prio);

//     return 0;
// }
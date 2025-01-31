#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <malloc.h>
#include "utils.h"

int main(void)
{
    int socket_desc;
    struct sockaddr_in server_addr;

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
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Send connection request to server:
    if (connect(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Unable to connect\n");
        return -1;
    }
    printf("Connected with server successfully\n");

    char buffer[1024];
    // Get input from the user:
    message_container_t *msg = (message_container_t *)malloc(sizeof(message_container_t));
    while (1)
    {
        memset(msg, 0, sizeof(message_container_t));
        printf("Enter filename: ");
        scanf("%s", msg->filename);
        printf("Enter start byte: ");
        scanf("%lu", &msg->start_byte);
        printf("Enter number of bytes: ");
        scanf("%lu", &msg->size_bytes);
        fflush(stdout);
        printf("Enter opcode: ");
        msg->op = getchar();
        msg->op = getchar();
        getchar();
        send(socket_desc, msg, sizeof(message_container_t), 0);
        if (msg->op == 'w')
        {
            memset(buffer, 0, sizeof(buffer));
            printf("Enter bytes to send: ");
            fgets(buffer, 1024, stdin);
            send(socket_desc, buffer, strlen(buffer), 0);
        }
        if (msg->op == 'r')
        {
            recv(socket_desc, buffer, msg->size_bytes, 0);
            buffer[msg->size_bytes] = '\0';
            printf("Recieved from server: %s\n", buffer);
        }
    }
    // Close the socket:
    close(socket_desc);

    return 0;
}
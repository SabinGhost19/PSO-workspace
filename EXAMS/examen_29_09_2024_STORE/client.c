#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 24330
int main(void)
{
    char param[1024];
    // strcpy(param, "ls -l");
    fgets(param, sizeof(param), stdin);
    char *save_ptr;
    char *args[10];
    int i = 0;
    printf("%s.....\n", param);
    param[strlen(param) - 1] = '\0';
    char *p = strtok_r(param, " ", &save_ptr);
    while (p != NULL)
    {
        args[i++] = p;
        p = strtok_r(NULL, " ", &save_ptr);
    }
    args[i] = NULL;
    for (int j = 0; args[j] != NULL; j++)
    {
        printf("%s .....\n", args[j]); // ----nu afiseaza
    }

    // int socket_desc;
    // struct sockaddr_in server_addr;
    // char server_message[2000], client_message[2000];

    // // Clean buffers:
    // memset(server_message, '\0', sizeof(server_message));
    // memset(client_message, '\0', sizeof(client_message));

    // // Create socket, we use SOCK_STREAM for TCP
    // socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    // if (socket_desc < 0)
    // {
    //     printf("Unable to create socket\n");
    //     return -1;
    // }

    // printf("Socket created successfully\n");

    // // Set port and IP the same as server-side:
    // server_addr.sin_family = AF_INET;
    // server_addr.sin_port = htons(PORT);
    // server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // // Send connection request to server:
    // if (connect(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    // {
    //     printf("Unable to connect\n");
    //     return -1;
    // }
    // printf("Connected with server successfully\n");

    // // Get input from the user:
    // printf("Enter message: ");
    // fgets(client_message, sizeof(client_message), stdin);
    // printf("Ai introdus: %s....\n", client_message);
    // // Send the message to server:
    // if (send(socket_desc, client_message, strlen(client_message), 0) < 0)
    // {
    //     printf("Unable to send message\n");
    //     return -1;
    // }

    // // Receive the server's response:
    // if (recv(socket_desc, server_message, sizeof(server_message), 0) < 0)
    // {
    //     printf("Error while receiving server's msg\n");
    //     return -1;
    // }

    // printf("Server's response: %s\n", server_message);

    // // Close the socket:
    // close(socket_desc);

    return 0;
}

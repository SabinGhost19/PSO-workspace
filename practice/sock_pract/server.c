#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(void)
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

    // Accept an incoming connection from one of the clients:
    client_size = sizeof(client_addr);
    client_sock = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);

    if (client_sock < 0)
    {
        printf("Can't accept\n");
        return -1;
    }
    printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // Receive client's message:
    // We now use client_sock, not socket_desc
    if (recv(client_sock, client_message, sizeof(client_message), 0) < 0)
    {
        printf("Couldn't receive\n");
        return -1;
    }
    printf("Msg from client: %s\n", client_message);

    // Respond to client:
    strcpy(server_message, "This is the server's message.");

    if (send(client_sock, server_message, strlen(server_message), 0) < 0)
    {
        printf("Can't send\n");
        return -1;
    }

    int listenfd, sockfd;
    struct epoll_event ev;

    epfd = epoll_create(EPOLL_INIT_BACKSTORE);

    /* read user data from standard input */
    ev.data.fd = STDIN_FILENO; /* key is file descriptor */
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);

    /* TODO ...  create server socket (listener) */

    /* add listener socket */
    ev.data.fd = listenfd; /* key is file descriptor */
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

    while (1)
    { /* server loop */
        struct epoll_event ret_ev;

        /* wait for readiness notification */
        epoll_wait(epfd, &ret_ev, 1, -1);

        if ((ret_ev.data.fd == listenfd) && ((ret_ev.events & EPOLLIN) != 0))
        {
            /* TODO ... handle new connection */
        }
        else if ((ret_ev.data.fd == STDIN_FILENO) &&
                 ((ret_ev.events & EPOLLIN) != 0))
        {
            /* TODO ... read user data from standard input */
        }
        else
        {
            /* TODO ... handle message on connection sockets */
        }
    }
    [...]

        // Closing the socket:
        close(client_sock);
    close(socket_desc);

    return 0;
}
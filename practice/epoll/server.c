#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <string.h>

#define MAX_EVENTS 10
#define PORT 8080
#define BACKLOG 10

// Funcția care va trata conexiunea clientului
void handle_client(int client_fd)
{
    char buffer[1024];
    ssize_t bytes_read;

    // Citește datele de la client
    bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0)
    {
        buffer[bytes_read] = '\0';
        printf("Received: %s\n", buffer);

        // Trimite un răspuns înapoi la client
        write(client_fd, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, world!", 66);
    }

    // Închide conexiunea cu clientul
    close(client_fd);
}

int server_fd, client_fd, epoll_fd;
struct sockaddr_in server_addr;
struct epoll_event ev, events[MAX_EVENTS];

void configure_socket_init()
{
    // Crearea unui socket pentru server
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("socket");
        exit(1);
    }

    // Configurarea adresei serverului
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Asocierea socketului cu adresa și portul
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind");
        exit(1);
    }

    // Ascultarea conexiunilor de intrare
    if (listen(server_fd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }
}
int main()
{
    configure_socket_init();

    // Crearea unui obiect epoll
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        perror("epoll_create1");
        exit(1);
    }

    // Setarea evenimentului pentru server_fd (ascultă pentru conexiuni)
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1)
    {
        perror("epoll_ctl");
        exit(1);
    }

    // Loop-ul principal care așteaptă evenimente
    while (1)
    {
        // Așteaptă evenimentele
        printf("Epoll wait ....\n");
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1)
        {
            perror("epoll_wait");
            exit(1);
        }
        printf("Epoll DONE waiting... ....\n");

        // Procesează fiecare eveniment
        for (int i = 0; i < nfds; i++)
        {
            if (events[i].data.fd == server_fd)
            {
                // Este un eveniment pe socket-ul serverului (nouă conexiune)
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);

                // Acceptă conexiunea
                client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
                if (client_fd == -1)
                {
                    perror("accept");
                    continue;
                }

                // Setează evenimentul pentru client_fd (pentru citire)
                ev.events = EPOLLIN;
                ev.data.fd = client_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
                {
                    perror("epoll_ctl");
                    exit(1);
                }
                printf("New connection: %d\n", client_fd);
            }
            else if (events[i].events & EPOLLIN)
            {
                // Este un eveniment de citire pe un client (date disponibile)
                handle_client(events[i].data.fd);
            }
        }
    }

    close(server_fd);
    return 0;
}

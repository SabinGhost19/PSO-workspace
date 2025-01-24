#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

pthread_mutex_t tank_mutex = PTHREAD_MUTEX_INITIALIZER;
int tank;
pthread_cond_t condFuel = PTHREAD_COND_INITIALIZER;

void *filling_tank(void *args)
{
    for (int i = 0; i < 5; i++)
    {
        printf("Filling the TANK.....\n");
        pthread_mutex_lock(&tank_mutex);
        tank += 10;
        pthread_mutex_unlock(&tank_mutex);
        pthread_cond_signal(&condFuel);
        sleep(1);
    }
}
void *car(void *args)
{
    pthread_mutex_lock(&tank_mutex);
    while (tank < 40)
    {
        printf("waiting car.................\n");
        pthread_cond_wait(&condFuel, &tank_mutex);
    }
    printf("Got the FUEL....CAR DRIVING\n");
    tank -= 50;
    pthread_mutex_unlock(&tank_mutex);
}
int main()
{

    pthread_t td[2];

    // creating
    for (int i = 0; i < 2; i++)
    {
        if (i == 1)
        {
            if (pthread_create(&td[i], NULL, &filling_tank, NULL) != 0)
            {
                perror("Thread creation failed");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            if (pthread_create(&td[i], NULL, &car, NULL) != 0)
            {
                perror("Thread creation failed");
                exit(EXIT_FAILURE);
            }
        }
    }
    // joining
    for (int i = 0; i < 2; i++)
    {
        if (pthread_join(td[i], NULL) != 0)
        {
            perror("Thread join failed");
            exit(EXIT_FAILURE);
        }
    }
    pthread_mutex_destroy(&tank_mutex);
    pthread_cond_destroy(&condFuel);
}

// #include <stdio.h>
// #include <string.h>
// #include <pthread.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <time.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>

// #define THREAD_NUM 4
// #define PORT 8080
// #define MAX_CLIENTS 100

// typedef struct Task
// {
//     int client_socket;
// } Task;

// Task taskQueue[MAX_CLIENTS];
// int taskCount = 0;

// pthread_mutex_t mutexQueue;
// pthread_cond_t condQueue;

// void executeTask(Task *task)
// {
//     char buffer[1024];
//     ssize_t bytes_read;

//     // Citește datele de la client
//     bytes_read = read(task->client_socket, buffer, sizeof(buffer) - 1);
//     if (bytes_read > 0)
//     {
//         buffer[bytes_read] = '\0';
//         printf("Received from client: %s\n", buffer);
//         // Trimite răspuns la client
//         write(task->client_socket, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, world!", 66);
//     }

//     // Închide conexiunea clientului
//     close(task->client_socket);
// }

// void submitTask(Task task)
// {
//     pthread_mutex_lock(&mutexQueue);
//     taskQueue[taskCount] = task;
//     taskCount++;
//     pthread_mutex_unlock(&mutexQueue);
//     pthread_cond_signal(&condQueue);
// }

// void *startThread(void *args)
// {
//     while (1)
//     {
//         Task task;

//         pthread_mutex_lock(&mutexQueue);
//         while (taskCount == 0)
//         {
//             pthread_cond_wait(&condQueue, &mutexQueue);
//         }

//         task = taskQueue[0];
//         int i;
//         for (i = 0; i < taskCount - 1; i++)
//         {
//             taskQueue[i] = taskQueue[i + 1];
//         }
//         taskCount--;
//         pthread_mutex_unlock(&mutexQueue);

//         executeTask(&task);
//     }
// }

// int main(int argc, char *argv[])
// {
//     pthread_t th[THREAD_NUM];
//     pthread_mutex_init(&mutexQueue, NULL);
//     pthread_cond_init(&condQueue, NULL);

//     // Crearea threadurilor din pool
//     int i;
//     for (i = 0; i < THREAD_NUM; i++)
//     {
//         if (pthread_create(&th[i], NULL, &startThread, NULL) != 0)
//         {
//             perror("Failed to create the thread");
//             exit(1);
//         }
//     }

//     // Crearea serverului
//     int server_fd, client_fd;
//     struct sockaddr_in server_addr, client_addr;
//     socklen_t client_size = sizeof(client_addr);

//     server_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_fd == -1)
//     {
//         perror("Socket failed");
//         exit(1);
//     }

//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = INADDR_ANY;
//     server_addr.sin_port = htons(PORT);

//     if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
//     {
//         perror("Bind failed");
//         exit(1);
//     }

//     if (listen(server_fd, MAX_CLIENTS) == -1)
//     {
//         perror("Listen failed");
//         exit(1);
//     }

//     printf("Server listening on port %d...\n", PORT);

//     while (1)
//     {
//         client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_size);
//         if (client_fd == -1)
//         {
//             perror("Accept failed");
//             continue;
//         }

//         // Adaugă clientul în coada de taskuri
//         Task task = {client_fd};
//         submitTask(task);
//     }

//     // Așteaptă terminarea threadurilor (pentru simplificare, doar ca exemplu)
//     for (i = 0; i < THREAD_NUM; i++)
//     {
//         if (pthread_join(th[i], NULL) != 0)
//         {
//             perror("Failed to join the thread");
//         }
//     }

//     pthread_mutex_destroy(&mutexQueue);
//     pthread_cond_destroy(&condQueue);

//     close(server_fd);
//     return 0;
// }

// #include <stdio.h>
// #include <string.h>
// #include <pthread.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <time.h>

// #define THREAD_NUM 4

// typedef struct Task
// {
//     int a, b;
// } Task;

// Task taskQueue[256];
// int taskCount = 0;

// pthread_mutex_t mutexQueue;
// pthread_cond_t condQueue;

// void executeTask(Task *task)
// {
//     usleep(50000);
//     int result = task->a + task->b;
//     printf("The sum of %d and %d is %d\n", task->a, task->b, result);
// }

// void submitTask(Task task)
// {
//     pthread_mutex_lock(&mutexQueue);
//     taskQueue[taskCount] = task;
//     taskCount++;
//     pthread_mutex_unlock(&mutexQueue);
//     pthread_cond_signal(&condQueue);
// }

// // 1 2 3 4 5
// // 2 3 4 5

// void *startThread(void *args)
// {
//     while (1)
//     {
//         Task task;

//         pthread_mutex_lock(&mutexQueue);
//         while (taskCount == 0)
//         {
//             pthread_cond_wait(&condQueue, &mutexQueue);
//         }

//         task = taskQueue[0];
//         int i;
//         for (i = 0; i < taskCount - 1; i++)
//         {
//             taskQueue[i] = taskQueue[i + 1];
//         }
//         taskCount--;
//         pthread_mutex_unlock(&mutexQueue);
//         executeTask(&task);
//     }
// }

// int main(int argc, char *argv[])
// {
//     pthread_t th[THREAD_NUM];
//     pthread_mutex_init(&mutexQueue, NULL);
//     pthread_cond_init(&condQueue, NULL);
//     int i;
//     for (i = 0; i < THREAD_NUM; i++)
//     {
//         if (pthread_create(&th[i], NULL, &startThread, NULL) != 0)
//         {
//             perror("Failed to create the thread");
//         }
//     }

//     srand(time(NULL));
//     for (i = 0; i < 100; i++)
//     {
//         Task t = {
//             .a = rand() % 100,
//             .b = rand() % 100};
//         submitTask(t);
//         printf("Task nr:%d \n", i);
//     }

//     for (i = 0; i < THREAD_NUM; i++)
//     {
//         if (pthread_join(th[i], NULL) != 0)
//         {
//             perror("Failed to join the thread");
//         }
//     }

//     pthread_mutex_destroy(&mutexQueue);
//     pthread_cond_destroy(&condQueue);
//     return 0;
// }

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/epoll.h>
// #include <netinet/in.h>
// #include <string.h>

// #define MAX_EVENTS 10
// #define PORT 8080
// #define BACKLOG 10

// // Funcția care va trata conexiunea clientului
// void handle_client(int client_fd)
// {
//     char buffer[1024];
//     ssize_t bytes_read;

//     // Citește datele de la client
//     bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
//     if (bytes_read > 0)
//     {
//         buffer[bytes_read] = '\0';
//         printf("Received: %s\n", buffer);

//         // Trimite un răspuns înapoi la client
//         write(client_fd, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, world!", 66);
//     }

//     // Închide conexiunea cu clientul
//     close(client_fd);
// }

// int server_fd, client_fd, epoll_fd;
// struct sockaddr_in server_addr;
// struct epoll_event ev, events[MAX_EVENTS];

// void configure_socket_init()
// {
//     // Crearea unui socket pentru server
//     server_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_fd == -1)
//     {
//         perror("socket");
//         exit(1);
//     }

//     // Configurarea adresei serverului
//     memset(&server_addr, 0, sizeof(server_addr));
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = INADDR_ANY;
//     server_addr.sin_port = htons(PORT);

//     // Asocierea socketului cu adresa și portul
//     if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
//     {
//         perror("bind");
//         exit(1);
//     }

//     // Ascultarea conexiunilor de intrare
//     if (listen(server_fd, BACKLOG) == -1)
//     {
//         perror("listen");
//         exit(1);
//     }
// }
// int main()
// {
//     configure_socket_init();

//     // Crearea unui obiect epoll
//     epoll_fd = epoll_create1(0);
//     if (epoll_fd == -1)
//     {
//         perror("epoll_create1");
//         exit(1);
//     }

//     // Setarea evenimentului pentru server_fd (ascultă pentru conexiuni)
//     ev.events = EPOLLIN;
//     ev.data.fd = server_fd;

//     if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1)
//     {
//         perror("epoll_ctl");
//         exit(1);
//     }

//     // Loop-ul principal care așteaptă evenimente
//     while (1)
//     {
//         // Așteaptă evenimentele
//         printf("Epoll wait ....\n");
//         int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
//         if (nfds == -1)
//         {
//             perror("epoll_wait");
//             exit(1);
//         }
//         printf("Epoll DONE waiting... ....\n");

//         // Procesează fiecare eveniment
//         for (int i = 0; i < nfds; i++)
//         {
//             if (events[i].data.fd == server_fd)
//             {
//                 // Este un eveniment pe socket-ul serverului (nouă conexiune)
//                 struct sockaddr_in client_addr;
//                 socklen_t client_len = sizeof(client_addr);

//                 // Acceptă conexiunea
//                 client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
//                 if (client_fd == -1)
//                 {
//                     perror("accept");
//                     continue;
//                 }

//                 // Setează evenimentul pentru client_fd (pentru citire)
//                 ev.events = EPOLLIN;
//                 ev.data.fd = client_fd;
//                 if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
//                 {
//                     perror("epoll_ctl");
//                     exit(1);
//                 }
//                 printf("New connection: %d\n", client_fd);
//             }
//             else if (events[i].events & EPOLLIN)
//             {
//                 // Este un eveniment de citire pe un client (date disponibile)
//                 handle_client(events[i].data.fd);
//             }
//         }
//     }

//     close(server_fd);
//     return 0;
// }

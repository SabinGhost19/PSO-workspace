#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define MAX_THREADS 4
#define MAX_TASKS 5
#define FILENAME_DIM 30
typedef struct Task
{
    int sum;
    char filename[FILENAME_DIM]
} Task;
Task task_vector[MAX_TASKS];

int vector_COUNT__ = 0;
void init(int argc);
void read_files(int argc, char *argv[]);
void submit_Task(Task new_task);
pthread_mutex_t _task_mutex_ = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t _task_cond = PTHREAD_COND_INITIALIZER;

void execute_the_task(Task task_to_execute)
{
    printf("EXECUTING THE TASk: %s\n", task_to_execute.filename);
}
void *starting_routine(void *args)
{
    while (1)
    {

        Task taken_task;
        pthread_mutex_lock(&_task_mutex_);
        while (vector_COUNT__ == 0)
        {
            pthread_cond_wait(&_task_cond, &_task_mutex_);
        }

        taken_task = task_vector[0];
        for (int i = 0; i < vector_COUNT__; i++)
        {
            task_vector[i] = task_vector[i + 1];
        }
        vector_COUNT__--;
        pthread_mutex_unlock(&_task_mutex_);
        execute_the_task(taken_task);
    }
}
int main(int argc, char *argv[])
{
    init(argc);
    pthread_t tid[MAX_THREADS];
    read_files(argc, argv);
    for (int i = 0; i < MAX_THREADS; i++)
    {
        if (pthread_create(&tid[i], NULL, &starting_routine, NULL) != 0)
        {
            perror("Error threads create");
            exit(-1);
        }
    }

    for (int i = 0; i < MAX_THREADS; i++)
    {
        if (pthread_join(tid[i], NULL) != 0)
        {
            perror("Error threads join");
            exit(-1);
        }
    }

    return 0;
}
void read_files(int argc, char *argv[])
{

    char filename[1024];
    printf("ARGVs: 0:%s....1: %s...2: %s\n", argv[0], argv[1], argv[2]);

    strcpy(filename, argv[1]);
    printf("FILENAME: %s \n", filename);

    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("open file error");
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    int bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0)
    {
        perror("0 bytes read error");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_read] = '\0';

    printf("Bytes Read: %d\n", bytes_read);
    printf("Data read from file: %s\n", buffer);

    char *start = strtok(buffer, "\n");
    while (start != NULL)
    {
        Task new_task4;
        new_task4.sum = 0;
        strcpy(new_task4.filename, start);
        printf("Filename found: %s\n", new_task4.filename);
        submit_Task(new_task4);
        start = strtok(NULL, "\n");
    }

    close(fd);
}

void submit_Task(Task new_task)
{
    pthread_mutex_lock(&_task_mutex_);
    task_vector[vector_COUNT__] = new_task;
    vector_COUNT__++;
    pthread_mutex_unlock(&_task_mutex_);
    pthread_cond_signal(&_task_cond);
}

void init(int argc)
{
    if (argc < 2)
    {
        perror("Argument not given");
        exit(-1);
    }
}

// #include <stdio.h>
// #include <unistd.h>
// #include <pthread.h>
// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <semaphore.h>

// #define MAX_THREADS 1000
// #define PORT 8080
// #define MAX_TASKS 2

// int socket_desc, client_sock, client_size;
// struct sockaddr_in server_addr, client_addr;
// char server_message[2000], client_message[2000];
// pthread_mutex_t task_mutex = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t task_cond = PTHREAD_COND_INITIALIZER;
// pthread_cond_t task_Max_cond = PTHREAD_COND_INITIALIZER;
// sem_t semaForr;
// int nr_thread_count;

// typedef struct Task
// {
//     int client_socket;
//     char client_message[2000]
// } Task;

// Task taskQueue[10000];
// int task_count = 0;
// void init_the_connection();
// void respond(const char *message, int cs);
// void handle_request(Task new_Task)
// {
//     // Receive client's message:
//     // We now use client_sock, not socket_desc
//     int recv_nr = recv(new_Task.client_socket, new_Task.client_message, sizeof(new_Task.client_message), 0);
//     if (recv_nr <= 0)
//     {
//         printf("Couldn't receive\n");
//         exit(-1);
//     }
//     new_Task.client_message[recv_nr] = '\0';
//     printf("Msg from client lenght:%d ......: %s\n", recv_nr, new_Task.client_message);
//     respond("SALUTTTTTTT", new_Task.client_socket);
// }

// void *start_routine(void *args)
// {
//     sem_wait(&semaForr);
//     printf("Thread executing....%d\n", nr_thread_count);
//     Task new_task;
//     pthread_mutex_lock(&task_mutex);
//     while (task_count == 0)
//     {
//         pthread_cond_wait(&task_cond, &task_mutex);
//     }

//     // shift all to the right
//     new_task = taskQueue[0];

//     for (int i = 0; i < task_count; i++)
//     {
//         taskQueue[i] = taskQueue[i + 1];
//     }
//     task_count--;
//     // take a task and execute
//     pthread_mutex_unlock(&task_mutex);
//     handle_request(new_task);
//     sem_post(&semaForr);
// }
// int main(int argc, char *argv[])
// {

//     pthread_t thread_poll[MAX_THREADS];
//     sem_init(&semaForr, 0, MAX_THREADS);

//     init_the_connection();

//     int i = 0;
//     while (1)
//     {
//         // Accept an incoming connection from one of the clients :
//         client_size = sizeof(client_addr);
//         client_sock = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);
//         if (client_sock < 0)
//         {
//             printf("Can't accept\n");
//             return -1;
//         }
//         printf("Client: CURRENT TASKS: %d connected at IP: %s and port: %i\n", task_count, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

//         Task new_task;
//         new_task.client_socket = client_sock;
//         pthread_mutex_lock(&task_mutex);
//         taskQueue[task_count] = new_task;
//         task_count++;
//         nr_thread_count++;
//         pthread_mutex_unlock(&task_mutex);
//         pthread_cond_signal(&task_cond);

//         if (pthread_create(&thread_poll[i], NULL, &start_routine, NULL) != 0)
//         {
//             perror("Error at creating thread");
//             exit(-1);
//         }

//         // while (task_count >= MAX_TASKS)
//         // {
//         //     printf("CAN'T ACCEPT THE CONNECTION.....\n");
//         //     pthread_cond_wait(&task_Max_cond, &task_mutex);
//         // }
//     }

//     for (int i = 0; i < MAX_THREADS; i++)
//     {
//         if (pthread_join(thread_poll[i], NULL) != 0)
//         {
//             perror("THreads join error");
//             exit(EXIT_FAILURE);
//         }
//     }

//     // Closing the socket:
//     pthread_cond_destroy(&task_cond);
//     pthread_mutex_destroy(&task_mutex);
//     close(socket_desc);
//     return 0;
// }

// void respond(const char *message, int client_sock_t)
// {
//     // Respond to client:
//     strcpy(server_message, message);

//     if (send(client_sock_t, server_message, strlen(server_message), 0) < 0)
//     {
//         printf("Can't send\n");
//         exit(-1);
//     }
//     close(client_sock_t);
//     pthread_cond_signal(&task_Max_cond);
// }
// void init_the_connection()
// {
//     // Clean buffers:
//     memset(server_message, '\0', sizeof(server_message));
//     memset(client_message, '\0', sizeof(client_message));

//     // Create socket:
//     socket_desc = socket(AF_INET, SOCK_STREAM, 0);

//     if (socket_desc < 0)
//     {
//         printf("Error while creating socket\n");
//         exit(-1);
//     }

//     printf("Socket created successfully\n");

//     // Set port and IP that we'll be listening for, any other IP_SRC or port will be dropped:
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_port = htons(PORT);
//     server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

//     // Bind to the set port and IP:
//     if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
//     {
//         printf("Couldn't bind to the port\n");
//         exit(-1);
//     }
//     printf("Done with binding\n");

//     // Listen for clients:
//     if (listen(socket_desc, 1) < 0)
//     {
//         printf("Error while listening\n");
//         exit(-1);
//     }
//     printf("\nListening for incoming connections.....\n");
// }

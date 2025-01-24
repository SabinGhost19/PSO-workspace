#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_THREADS 4
#define PORT 8080
#define MAX_TASKS 2

int socket_desc, client_sock, client_size;
struct sockaddr_in server_addr, client_addr;
char server_message[2000], client_message[2000];
pthread_mutex_t task_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t task_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t task_Max_cond = PTHREAD_COND_INITIALIZER;

typedef struct Task
{
    int client_socket;
    char client_message[2000]
} Task;

Task taskQueue[MAX_TASKS];
int task_count = 0;
void init_the_connection();
void respond(const char *message, int cs);
void handle_request(Task new_Task)
{
    // Receive client's message:
    // We now use client_sock, not socket_desc
    int recv_nr = recv(new_Task.client_socket, new_Task.client_message, sizeof(new_Task.client_message), 0);
    if (recv_nr <= 0)
    {
        printf("Couldn't receive\n");
        exit(-1);
    }
    new_Task.client_message[recv_nr] = '\0';
    printf("Msg from client lenght:%d ......: %s\n", recv_nr, new_Task.client_message);
    respond("SALUTTTTTTT", new_Task.client_socket);
}
void *start_routine(void *args)
{
    while (1)
    {
        Task new_task;
        pthread_mutex_lock(&task_mutex);
        while (task_count == 0)
        {
            pthread_cond_wait(&task_cond, &task_mutex);
        }

        // shift all to the right
        new_task = taskQueue[0];

        for (int i = 0; i < task_count; i++)
        {
            taskQueue[i] = taskQueue[i + 1];
        }
        task_count--;
        // take a task and execute
        pthread_mutex_unlock(&task_mutex);
        handle_request(new_task);
    }
}
int main(int argc, char *argv[])
{

    pthread_t thread_poll[MAX_THREADS];

    init_the_connection();
    for (int i = 0; i < MAX_THREADS; i++)
    {
        if (pthread_create(&thread_poll[i], NULL, &start_routine, NULL) != 0)
        {

            perror("THreads start error");
            exit(EXIT_FAILURE);
        }
    }

    while (1)
    {
        // Accept an incoming connection from one of the clients :
        client_size = sizeof(client_addr);
        client_sock = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);
        if (client_sock < 0)
        {
            printf("Can't accept\n");
            return -1;
        }
        printf("Client: CURRENT TASKS: %d connected at IP: %s and port: %i\n", task_count, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        Task new_task;
        new_task.client_socket = client_sock;
        pthread_mutex_lock(&task_mutex);
        taskQueue[task_count] = new_task;
        task_count++;
        pthread_cond_signal(&task_cond);

        while (task_count >= MAX_TASKS)
        {
            printf("CAN'T ACCEPT THE CONNECTION.....\n");
            pthread_cond_wait(&task_Max_cond, &task_mutex);
        }

        pthread_mutex_unlock(&task_mutex);
    }

    for (int i = 0; i < MAX_THREADS; i++)
    {
        if (pthread_join(thread_poll[i], NULL) != 0)
        {
            perror("THreads join error");
            exit(EXIT_FAILURE);
        }
    }

    // Closing the socket:
    pthread_cond_destroy(&task_cond);
    pthread_mutex_destroy(&task_mutex);
    close(socket_desc);
    return 0;
}

void respond(const char *message, int client_sock_t)
{
    // Respond to client:
    strcpy(server_message, message);

    if (send(client_sock_t, server_message, strlen(server_message), 0) < 0)
    {
        printf("Can't send\n");
        exit(-1);
    }
    close(client_sock_t);
    pthread_cond_signal(&task_Max_cond);
}
void init_the_connection()
{
    // Clean buffers:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));

    // Create socket:
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0)
    {
        printf("Error while creating socket\n");
        exit(-1);
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
        exit(-1);
    }
    printf("Done with binding\n");

    // Listen for clients:
    if (listen(socket_desc, 1) < 0)
    {
        printf("Error while listening\n");
        exit(-1);
    }
    printf("\nListening for incoming connections.....\n");
}

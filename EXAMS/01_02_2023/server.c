#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#define READ_HEAD 0
#define WRITE_HEAD 1
#define BUF_SIZE 1024
#define MAX_COMANDS 4
pthread_mutex_t write_pipe_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t current_commands_in_list_MUTEX = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t comands_list_MUTEX = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t finished_mutex = PTHREAD_MUTEX_INITIALIZER;
int finished = 0;
int total_comands = 0;
sem_t pipe_semaphore;
sem_t add_commands;
sem_t take_commands;

char comands_list[MAX_COMANDS][1024];
int current_commands_in_list = 0;
void show_list();
char *generate_command()
{
    char *command = (char *)malloc(sizeof(char) * (100));
    int cod = rand() % 3 + 1;
    int pid = getpid();
    sprintf(command, "%d %d ", pid, cod);
    int len = strlen(command);
    int i = 0;
    for (i = len; i < len + 10; i++)
    {
        command[i] = rand() % 57 + 65;
    }
    command[i] = '\0';
    return command;
}
void child_functio(int *pipefd)
{
    srand(time(NULL));
    // pid //cod //parametru[10]
    // wait random 0-1000ms
    char *command = NULL;
    for (int i = 0; i < 5; i++)
    {
        usleep((rand() % 1000) * 1000);
        command = generate_command();
        // printf("Command in chidl:%s...\n", command);

        pthread_mutex_lock(&write_pipe_mutex);
        int rc = write(pipefd[WRITE_HEAD], command, strlen(command));
        pthread_mutex_unlock(&write_pipe_mutex);
        // sem_post(&pipe_semaphore);
    }
    printf("Child terminated...\n");
}
void *worker_handling(void *args)
{
    char command[BUF_SIZE];
    pthread_mutex_lock(&finished_mutex);
    while (finished != 1)
    {
        pthread_mutex_unlock(&finished_mutex);

        sem_wait(&add_commands);
        pthread_mutex_lock(&finished_mutex);
        if (finished == 1)
        {
            printf("EXIT THREAD_inline\n");
            pthread_mutex_unlock(&finished_mutex);
            pthread_exit(NULL);
        }
        pthread_mutex_unlock(&finished_mutex);

        pthread_mutex_lock(&comands_list_MUTEX);
        pthread_mutex_lock(&current_commands_in_list_MUTEX);
        strcpy(command, comands_list[0]);
        for (int i = 0; i < current_commands_in_list; i++)
        {
            strcpy(comands_list[i], comands_list[i + 1]);
        }
        current_commands_in_list--;
        pthread_mutex_unlock(&comands_list_MUTEX);
        pthread_mutex_unlock(&current_commands_in_list_MUTEX);
        sem_post(&take_commands);
        printf("Thread...take command:%s\n", command);
        pthread_mutex_lock(&finished_mutex);
    }
    pthread_mutex_unlock(&finished_mutex);
    printf("EXIT THREAD\n");
    pthread_exit(NULL);
    return NULL;
}
int main()
{

    pthread_t td[2];
    int params[2];
    sem_init(&pipe_semaphore, 1, 0);
    sem_init(&add_commands, 0, 0);
    sem_init(&take_commands, 0, MAX_COMANDS);
    for (int i = 0; i < 2; i++)
    {
        params[i] = i;
        pthread_create(&td[i], NULL, &worker_handling, &params[i]);
    }
    // creating children
    // makdin the pipe
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 3; i++)
    {
        pid_t pid = fork();
        switch (pid)
        {
        case 0:
        {
            // children:
            close(pipefd[READ_HEAD]);
            child_functio(pipefd);
            exit(0);
        }
        case -1:
        {
            perror("error at fork");
            exit(EXIT_FAILURE);
        }
        default:
        }
    }
    char buffer[BUF_SIZE];
    close(pipefd[WRITE_HEAD]);
    for (int i = 0; i < 15; i++)
    {
        // sem_wait(&pipe_semaphore);
        sem_wait(&take_commands);
        int rc = read(pipefd[READ_HEAD], buffer, 18);
        // printf("....received2222:::...%s\n", buffer);
        buffer[18] = '\0';
        // printf("cirtire....%s...\n", buffer);

        pthread_mutex_lock(&comands_list_MUTEX);
        pthread_mutex_lock(&current_commands_in_list_MUTEX);

        strcpy(comands_list[current_commands_in_list], buffer);
        current_commands_in_list++;
        // printf("index: %d\n", current_commands_in_list);
        total_comands++;
        pthread_mutex_unlock(&comands_list_MUTEX);
        pthread_mutex_unlock(&current_commands_in_list_MUTEX);
        sem_post(&add_commands);
        // show_list();
    }
    printf("Print: Total comands: %d\n", total_comands);
    int status;
    for (int i = 0; i < 3; i++)
    {
        wait(&status);
        printf("Proc Child status: %d\n", status);
    }
    pthread_mutex_lock(&finished_mutex);
    finished = 1;
    pthread_mutex_unlock(&finished_mutex);
    sem_post(&add_commands);
    sem_post(&add_commands);

    for (int i = 0; i < 2; i++)
    {
        params[i] = i;
        pthread_join(td[i], NULL);
    }
    printf("DEstroingggg....\n");
    sem_destroy(&pipe_semaphore);
    sem_destroy(&add_commands);
    sem_destroy(&take_commands);
    pthread_mutex_destroy(&comands_list_MUTEX);
    pthread_mutex_destroy(&current_commands_in_list_MUTEX);
    return 0;
}
void show_list()
{
    pthread_mutex_lock(&comands_list_MUTEX);
    pthread_mutex_lock(&current_commands_in_list_MUTEX);

    for (int i = 0; i < current_commands_in_list; i++)
    {
        printf("Comand SHOW all:%s''''''\n", comands_list[i]);
    }
    pthread_mutex_unlock(&comands_list_MUTEX);
    pthread_mutex_unlock(&current_commands_in_list_MUTEX);
}

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/select.h>
#include <errno.h>

#define MESSAGE_LENGTH 256
#define NUM_THREADS 6
#define FOR_THREADS for (int i = 0; i < NUM_THREADS; i++)

int finished = 0;
pthread_mutex_t mutex_finish = PTHREAD_MUTEX_INITIALIZER;

int thread_counter = 0;
pthread_mutex_t mutex_threadCounter = PTHREAD_MUTEX_INITIALIZER;

int pipe_fd[2];

void *thread_routine(void *args)
{
    int client_socket = *((int *)args);
    printf("Threadul a preluat conexiunea pentru clientul: %d\n", client_socket);

    fd_set rdfs;
    struct timeval tval;
    int retval = 0;

    char buffer[MESSAGE_LENGTH];

    while (1)
    {
        FD_ZERO(&rdfs);
        FD_SET(client_socket, &rdfs);
        FD_SET(pipe_fd[0], &rdfs);

        int max_fd = client_socket > pipe_fd[0] ? client_socket : pipe_fd[0];

        tval.tv_sec = 1;
        tval.tv_usec = 0;

        retval = select(max_fd + 1, &rdfs, NULL, NULL, &tval);
        if (retval == -1)
        {
            printf("way 1\n");
            if (errno != EINTR)
            {
                perror("Eroare la select\n");
                exit(EXIT_FAILURE);
            }
        }
        else if (retval == 0)
        {
            // pthread_mutex_lock(&mutex_finish);
            printf("way 2\n");
            continue;
        }
        else
        {
            if (FD_ISSET(pipe_fd[0], &rdfs))
            {
                printf("Notificare de terminare primită prin pipe.\n");
                break;
            }

            ssize_t bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes_read < 1)
            {
                perror("Eroare la receive!\n");
                exit(EXIT_FAILURE);
            }

            printf("Mesaj primit de la client: %s", buffer);
            break;
        }
    }

    printf("Thread terminat!\n");
    pthread_exit(NULL);
    return NULL;
}

void sigint_handler()
{
    printf("Primire semnal SIGINT ... Start graceful termination... BYE #_#\n");
    pthread_mutex_lock(&mutex_finish);
    finished = 1;
    pthread_mutex_unlock(&mutex_finish);

    if (write(pipe_fd[1], "x", 1) == -1)
    {
        perror("Eroare la scrierea în pipe");
    }
}

int main(void)
{
    printf("PID: %d \n", getpid());

    if (pipe(pipe_fd) < 0)
    {
        perror("Eroare la crearea pipeului!\n");
        exit(EXIT_FAILURE);
    }

    int socket_desc, client_sock;
    socklen_t client_size;
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
    server_addr.sin_port = htons(2006);
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

    ///////////////// ^ CONFIGURARE SERVER ^ ////////////////////////////////////////

    struct sigaction sa_sigint;
    memset(&sa_sigint, 0, sizeof(sigaction));
    sa_sigint.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa_sigint, 0);

    pthread_t threads[NUM_THREADS];
    int client_sockets[NUM_THREADS];

    fd_set rfds;
    struct timeval tval;
    int retval = 0;

    pthread_mutex_lock(&mutex_finish);
    while (!finished)
    {
        pthread_mutex_unlock(&mutex_finish);
        tval.tv_sec = 1;
        tval.tv_usec = 0;

        FD_ZERO(&rfds);
        FD_SET(socket_desc, &rfds);

        retval = select(socket_desc + 1, &rfds, NULL, NULL, &tval);
        if (retval == -1)
        {
            if (errno == EINTR)
            {
                // Ignorăm eroarea dacă a fost întrerupt de un semnal
                pthread_mutex_lock(&mutex_finish);
                continue;
            }

            perror("Eroare la select!\n");
            exit(EXIT_FAILURE);
        }
        else if (retval == 0)
        {
            pthread_mutex_lock(&mutex_finish);
            continue;
        }
        else
        {
            client_size = sizeof(client_addr);
            client_sock = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);

            if (client_sock < 0)
            {
                if (errno == EINTR)
                    continue;
                printf("Can't accept\n");
                return -1;
            }
            printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            pthread_mutex_lock(&mutex_threadCounter);
            int thread_index = thread_counter;
            thread_counter++;
            pthread_mutex_unlock(&mutex_threadCounter);

            client_sockets[thread_index] = client_sock;

            if (pthread_create(&threads[thread_index], NULL, thread_routine, &client_sockets[thread_index]) == -1)
            {
                perror("Eroare la crearea threadului!\n");
                exit(EXIT_FAILURE);
            }

            pthread_mutex_lock(&mutex_finish);
        }
    }

    pthread_mutex_lock(&mutex_threadCounter);
    int num_active_threads = thread_counter;
    pthread_mutex_unlock(&mutex_threadCounter);

    for (int i = 0; i < num_active_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    close(client_sock);
    close(socket_desc);

    printf("PROGRAM FINISHED!\n");

    return 0;
}

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/select.h>
#include <errno.h>

#define MAX_FILES 20
#define FILENAME_LENGTH 24
#define MESSAGE_LENGTH 256
#define NUM_THREADS 6
#define FOR_THREADS for (int i = 0; i < NUM_THREADS; i++)

typedef struct
{
    char files[MAX_FILES][FILENAME_LENGTH];
    int start;
    int end;
    int counter;
} File_list;

File_list file_list = {.start = 0, .end = 0, .counter = 0};
pthread_mutex_t mutex_fileList;

int finished = 0;
pthread_mutex_t mutex_finish = PTHREAD_MUTEX_INITIALIZER;

int thread_counter = 0;
pthread_mutex_t mutex_threadCounter = PTHREAD_MUTEX_INITIALIZER;

int pipe_fd[2];

void *thread_routine(void *args)
{
    int client_socket = *((int *)args);
    printf("Threadul a preluat conexiunea pentru clientul: %d\n", client_socket);

    fd_set rdfs;
    struct timeval tval;
    int retval = 0;

    char buffer[MESSAGE_LENGTH];

    while (1)
    {
        FD_ZERO(&rdfs);
        FD_SET(client_socket, &rdfs);
        FD_SET(pipe_fd[0], &rdfs);

        int max_fd = client_socket > pipe_fd[0] ? client_socket : pipe_fd[0];

        tval.tv_sec = 1;
        tval.tv_usec = 0;

        retval = select(max_fd + 1, &rdfs, NULL, NULL, &tval);
        if (retval == -1)
        {
            printf("way 1\n");
            if (errno != EINTR)
            {
                perror("Eroare la select\n");
                exit(EXIT_FAILURE);
            }
        }
        else if (retval == 0)
        {
            // pthread_mutex_lock(&mutex_finish);
            printf("way 2\n");
            continue;
        }
        else
        {
            if (FD_ISSET(pipe_fd[0], &rdfs))
            {
                printf("Notificare de terminare primită prin pipe.\n");
                break;
            }

            ssize_t bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes_read < 1)
            {
                perror("Eroare la receive!\n");
                exit(EXIT_FAILURE);
            }
            printf("Mesaj primit de la client: %s", buffer);

            char *ptr;
            char *token = strtok_r(buffer, ' ', &ptr);
            if (strcmp())

                break;
        }
    }

    printf("Thread terminat!\n");
    pthread_exit(NULL);
    return NULL;
}

void sigint_handler()
{
    printf("Primire semnal SIGINT ... Start graceful termination... BYE #_#\n");
    pthread_mutex_lock(&mutex_finish);
    finished = 1;
    pthread_mutex_unlock(&mutex_finish);

    if (write(pipe_fd[1], "x", 1) == -1)
    {
        perror("Eroare la scrierea în pipe");
    }
}

int main(void)
{
    printf("PID: %d \n", getpid());

    if (pipe(pipe_fd) < 0)
    {
        perror("Eroare la crearea pipeului!\n");
        exit(EXIT_FAILURE);
    }

    int socket_desc, client_sock;
    socklen_t client_size;
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
    server_addr.sin_port = htons(2006);
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

    ///////////////// ^ CONFIGURARE SERVER ^ ////////////////////////////////////////

    struct sigaction sa_sigint;
    memset(&sa_sigint, 0, sizeof(sigaction));
    sa_sigint.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa_sigint, 0);

    pthread_t threads[NUM_THREADS];
    int client_sockets[NUM_THREADS];

    fd_set rfds;
    struct timeval tval;
    int retval = 0;

    pthread_mutex_lock(&mutex_finish);
    while (!finished)
    {
        pthread_mutex_unlock(&mutex_finish);
        tval.tv_sec = 1;
        tval.tv_usec = 0;

        FD_ZERO(&rfds);
        FD_SET(socket_desc, &rfds);

        retval = select(socket_desc + 1, &rfds, NULL, NULL, &tval);
        if (retval == -1)
        {
            if (errno == EINTR)
            {
                // Ignorăm eroarea dacă a fost întrerupt de un semnal
                pthread_mutex_lock(&mutex_finish);
                continue;
            }

            perror("Eroare la select!\n");
            exit(EXIT_FAILURE);
        }
        else if (retval == 0)
        {
            pthread_mutex_lock(&mutex_finish);
            continue;
        }
        else
        {
            client_size = sizeof(client_addr);
            client_sock = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);

            if (client_sock < 0)
            {
                if (errno == EINTR)
                    continue;
                printf("Can't accept\n");
                return -1;
            }
            printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            pthread_mutex_lock(&mutex_threadCounter);
            int thread_index = thread_counter;
            thread_counter++;
            pthread_mutex_unlock(&mutex_threadCounter);

            client_sockets[thread_index] = client_sock;

            if (pthread_create(&threads[thread_index], NULL, thread_routine, &client_sockets[thread_index]) == -1)
            {
                perror("Eroare la crearea threadului!\n");
                exit(EXIT_FAILURE);
            }

            pthread_mutex_lock(&mutex_finish);
        }
    }

    pthread_mutex_lock(&mutex_threadCounter);
    int num_active_threads = thread_counter;
    pthread_mutex_unlock(&mutex_threadCounter);

    for (int i = 0; i < num_active_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    close(client_sock);
    close(socket_desc);

    printf("PROGRAM FINISHED!\n");

    return 0;
}
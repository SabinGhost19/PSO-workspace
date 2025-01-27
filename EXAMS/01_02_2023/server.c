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
    // char buffer[BUF_SIZE];
    // // sleep(2);
    // //  reading from pipe
    // int rc = read(pipefd[READ_HEAD], buffer, sizeof(buffer));
    // printf("....received:::...%s\n", buffer);

    // rc = read(pipefd[READ_HEAD], buffer, sizeof(buffer));
    // printf("....received:::...%s\n", buffer);

    // rc = read(pipefd[READ_HEAD], buffer, sizeof(buffer));
    // printf("....received:::...%s\n", buffer);
    int status;
    for (int i = 0; i < 3; i++)
    {
        wait(&status);
        printf("Proc Child status: %d\n", status);
    }
    pthread_mutex_lock(&finished_mutex);
    finished = 1;
    pthread_mutex_unlock(&finished_mutex);

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
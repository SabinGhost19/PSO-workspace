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
pthread_mutex_t __task_mutex__ = PTHREAD_MUTEX_INITIALIZER;
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
        pthread_mutex_lock(&__task_mutex__);
        while (vector_COUNT__ == 0)
        {
            pthread_cond_wait(&_task_cond, &__task_mutex__);
        }

        taken_task = task_vector[0];
        for (int i = 0; i < vector_COUNT__; i++)
        {
            task_vector[i] = task_vector[i + 1];
        }
        vector_COUNT__--;
        pthread_mutex_unlock(&__task_mutex__);
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
    pthread_mutex_lock(&__task_mutex__);
    task_vector[vector_COUNT__] = new_task;
    vector_COUNT__++;
    pthread_mutex_unlock(&__task_mutex__);
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

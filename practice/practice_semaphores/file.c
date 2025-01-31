#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX_THREADS 10
#define WRITE_HEAD 1
#define READ_HEAD 0
#define SEM_NAME "/sem_ana"
int pipe__[2];
sem_t *sem;
void *thread_routine(void *args)
{
    sem_wait(sem);
    printf("After wait\n");
    char buffer[100];
    int id = pthread_self() % 100;
    sprintf(buffer, "sabin_message_%d \n", id);
    write(pipe__[WRITE_HEAD], buffer, sizeof(buffer));
    return NULL;
}
void child_proc()
{
    printf("CHILD...\n");
    for (int i = 0; i < 10; i++)
    {
        char buffer[100];
        int recv_bytes = read(pipe__[READ_HEAD], buffer, sizeof(buffer));
        buffer[recv_bytes] = '\0';
        printf("Rcvparent..:%s \n", buffer);
        sem_post(sem);
    }
}
int main()
{
    sem_unlink(SEM_NAME);
    pipe(pipe__);
    sem = sem_open(SEM_NAME, O_CREAT | O_RDWR, 0666, 1);
    pthread_t tid[12];
    for (int i = 0; i < 10; i++)
    {
        pthread_create(&tid[i], NULL, thread_routine, NULL);
    }
    pid_t pid = fork();

    if (pid == 0)
    {
        child_proc();
        exit(0);
    }
    if (pid == -1)
    {
        perror("error at fork");
        exit(EXIT_FAILURE);
    }
    wait(NULL);
    for (int i = 0; i < 10; i++)
    {
        pthread_join(tid[i], NULL);
    }
    // sleep(1);

    // sem_close(sem);
    // sem_unlink(SEM_NAME);

    return 0;
}
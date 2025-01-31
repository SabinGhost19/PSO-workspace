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
int pipe__[2];
sem_t *sem;
void *thread_routine(void *args)
{
    char buffer[100];
    // sleep(1);
    // printf("Scris in pipe: %s...\n", buffer);
    // int val = 0;
    // sem_getvalue(sem, &val);
    // printf("Sem val threads: %d\n", val);
    // sleep(1);
    sem_wait(sem);
    printf("sabin\n");
    int thread_id = pthread_self() % 1000;
    snprintf(buffer, sizeof(buffer), "Mesaj_de_la_%d \n", thread_id);
    write(pipe__[WRITE_HEAD], buffer, strlen(buffer));
    //  pthread_exit(NULL);
    return NULL;
}
void child_proc(int pipe__[2])
{

    close(pipe__[WRITE_HEAD]);
    char buffer[100];
    int val = 0;
    // while (1)
    // for (int i = 0; i < 10; i++)
    // {
    //     sem_getvalue(sem, &val);
    //     printf("Value %d\n", val);
    //     if (val == 0)
    //     {
    //         int bytes_read = read(pipe__[READ_HEAD], buffer, sizeof(buffer));
    //         sem_post(sem);
    //         buffer[bytes_read] = '\0';
    //         printf("Read: %d nr bytes\n", bytes_read);
    //         printf("Citit din pipe:%s\n", buffer);
    //         memset(buffer, 0, sizeof(buffer));
    //     }
    // }
    for (int i = 0; i < 10; i++)
    {
        sem_getvalue(sem, &val);
        printf("Value %d\n", val);
        if (val == 0)
        {
            int bytes_read = read(pipe__[READ_HEAD], buffer, 100);
            sem_post(sem);
            buffer[bytes_read] = '\0';
            printf("Read: %d nr bytes\n", bytes_read);
            printf("Citit din pipe:%s\n", buffer);
            memset(buffer, 0, sizeof(buffer));
        }
    }
}

int main()
{
    // sem_unlink("/sem_name");
    pthread_t tid[20];
    pipe(pipe__);
    sem = sem_open("/sem_name", O_CREAT | O_RDWR, 0666, 1);

    pid_t pid = fork();
    if (pid == 0)
    {
        // child_proc(pipe__);
        exit(0);
    }
    else if (pid == -1)
    {
        perror("error fork");
        exit(EXIT_FAILURE);
    }
    close(pipe__[READ_HEAD]);
    //
    //
    //
    //
    for (int i = 0; i < MAX_THREADS; i++)
    {
        pthread_create(&tid[i], NULL, thread_routine, NULL);
    }
    for (int i = 0; i < MAX_THREADS; i++)
    {
        pthread_join(tid[i], NULL);
    }

    wait(NULL);
    sem_unlink("/sem_name");
    printf("eND");
    return 0;
}
// char *generate_string()
// {
//     srand(getpid());
//     char *new_string = (char *)malloc(sizeof(char) * 10);
//     int i = 0;
//     for (i = 0; i < 10; i++)
//     {
//         new_string[i] = rand() % 57 + 64;
//     }
//     new_string[i] = '\0';

//     return new_string;
// }
// void *routine(void *args)
// {
//     int *pipe = *((int **)args);
//     char *buffer = generate_string();
//     sem_wait(sem);
//     printf("Scris thread...%s\n", buffer);
//     write(pipe[WRITE_HEAD], buffer, strlen(buffer));
//     pthread_exit(NULL);
// }
// void child_routine(int pipe__[2])
// {
//     pthread_t tid;
//     close(pipe__[READ_HEAD]);
//     char *buffer = generate_string();
//     pthread_create(&tid, NULL, routine, &pipe__);
//     sem_wait(sem);
//     write(pipe__[WRITE_HEAD], buffer, strlen(buffer));
//     printf("Sccris\n");
//     pthread_join(tid, NULL);
// }
// int main()
// {
//     sem_unlink(SEM_NAME);
//     sem = sem_open(SEM_NAME, O_CREAT | O_RDWR, 0666, 1);
//     if (sem == SEM_FAILED)
//     {
//         perror("sem_open failed");
//         exit(EXIT_FAILURE);
//     }

//     int sem_value;
//     if (sem_getvalue(sem, &sem_value) != 0)
//     {
//         perror("sem_getvalue failed");
//         exit(EXIT_FAILURE);
//     }
//     printf("Semafor valoare inițială: %d\n", sem_value);
//     int pipe__[2];
//     pipe(pipe__);
//     for (int i = 0; i < 4; i++)
//     {
//         pid_t pid = fork();

//         if (pid == -1)
//         {
//             perror("PIPE ERROR");
//             exit(EXIT_FAILURE);
//         }
//         else if (pid == 0)
//         {
//             child_routine(pipe__);
//             exit(0);
//         }
//         else
//         {
//         }
//     }

//     close(pipe__[WRITE_HEAD]);
//     char buffer[BUFSIZ];
//     while (1)
//     {
//         if (sem_getvalue(sem, &sem_value) != 0)
//         {
//             perror("sem_getvalue failed");
//             exit(EXIT_FAILURE);
//         }
//         if (sem_value == 0)
//         {
//             read(pipe__[READ_HEAD], buffer, sizeof(buffer));
//             sem_post(sem);
//             printf("Received::::%s\n", buffer);
//         }
//     }
//     wait(NULL);
//     sem_close(sem);
//     sem_unlink(SEM_NAME);
//     return 0;
// }
// #define _POSIX_C_SOURCE 199309L
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <signal.h>
// #include <time.h>

// void signal_handler_usr1(int sig)
// {
//     if (sig == SIGUSR1)
//     {
//         printf("Received SIGUSR1! Resetting timer...\n");

//         alarm(10);
//     }
// }

// void signal_handler_alarm(int sig)
// {
//     if (sig == SIGALRM)
//     {
//         printf("Timeout reached! No signal received in 10 seconds.\n");
//         exit(1);
//     }
// }

// int main()
// {

//     printf("%d\n", getpid());
//     struct sigaction sa_usr1;
//     sa_usr1.sa_handler = signal_handler_usr1;
//     sa_usr1.sa_flags = 0;
//     sigemptyset(&sa_usr1.sa_mask);
//     if (sigaction(SIGUSR1, &sa_usr1, NULL) == -1)
//     {
//         perror("Error setting up SIGUSR1 handler");
//         exit(1);
//     }

//     struct sigaction sa_alarm;
//     sa_alarm.sa_handler = signal_handler_alarm;
//     sa_alarm.sa_flags = 0;
//     sigemptyset(&sa_alarm.sa_mask);
//     if (sigaction(SIGALRM, &sa_alarm, NULL) == -1)
//     {
//         perror("Error setting up SIGALRM handler");
//         exit(1);
//     }

//     alarm(10);

//     printf("Waiting for SIGUSR1 or timeout...\n");

//     while (1)
//     {
//         pause();
//     }

//     return 0;
// }
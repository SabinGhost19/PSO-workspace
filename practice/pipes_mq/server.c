// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <semaphore.h>
// #include <fcntl.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <pthread.h>
// #include <sys/wait.h>

// #define SEM_NAME "/sem_example"
// #define WRITE_HEAD 1
// #define READ_HEAD 0
// #define BUFSIZ 1024

// // Declarația semaforului global
// sem_t *sem;
// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// // Funcția de generare a unui șir aleator de caractere
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

// // Funcția procesului copil
// void child_routine(int pipe__[2])
// {
//     close(pipe__[READ_HEAD]);         // Închidem capătul de citire al pipe-ului în procesul copil
//     char *buffer = generate_string(); // Generăm un șir aleator

//     printf("Child generated: %s\n", buffer);

//     // Sincronizare cu semaforul
//     sem_wait(sem); // Așteaptă ca semaforul să fie 1
//     printf("Child sending: %s\n", buffer);

//     pthread_mutex_lock(&mutex);                        // Blocăm accesul la pipe pentru scriere
//     write(pipe__[WRITE_HEAD], buffer, strlen(buffer)); // Scriem în pipe
//     pthread_mutex_unlock(&mutex);                      // Deblocăm accesul

//     free(buffer); // Eliberăm memoria alocată
// }

// // Funcția procesului părinte
// int main()
// {
//     // Deschidem semaforul cu nume (creăm semaforul cu valoare inițială 1)
//     sem = sem_open(SEM_NAME, O_CREAT | O_RDWR, 0666, 1);
//     if (sem == SEM_FAILED)
//     {
//         perror("sem_open failed");
//         exit(EXIT_FAILURE);
//     }

//     // Creăm pipe-ul
//     int pipe__[2];
//     if (pipe(pipe__) == -1)
//     {
//         perror("Pipe creation failed");
//         exit(EXIT_FAILURE);
//     }

//     pid_t pid = fork();
//     if (pid == -1)
//     {
//         perror("fork failed");
//         exit(EXIT_FAILURE);
//     }

//     if (pid == 0)
//     {
//         // Codul procesului copil
//         child_routine(pipe__);
//         exit(0);
//     }
//     else
//     {
//         sleep(1);
//         // Codul procesului părinte
//         close(pipe__[WRITE_HEAD]); // Închidem capătul de scriere al pipe-ului în procesul părinte

//         char buffer[BUFSIZ];
//         int i = 0;
//         int sem_value;

//         // Verificăm valoarea semaforului
//         if (sem_getvalue(sem, &sem_value) != 0)
//         {
//             perror("sem_getvalue failed");
//             exit(EXIT_FAILURE);
//         }

//         printf("Semaphore value: %d\n", sem_value);

//         // Așteptăm ca semaforul să fie 0 pentru a citi din pipe
//         if (sem_value == 0)
//         {
//             sem_post(sem); // Setăm semaforul înapoi la 1
//             read(pipe__[READ_HEAD], buffer, sizeof(buffer));
//             buffer[strcspn(buffer, "\0")] = 'S'; // Citim din pipe
//             printf("Parent received: %s\n", buffer);
//         }

//         // Închidem pipe-ul
//         close(pipe__[READ_HEAD]);

//         // Așteptăm terminarea procesului copil
//         wait(NULL);
//     }

//     // Închidem semaforul
//     sem_close(sem);
//     sem_unlink(SEM_NAME); // Eliberăm semaforul la final

//     return 0;
// }
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <semaphore.h>
#define MQ_NAME "/my_message_queue"
#define SEM_NAME "/sem_"
#define WRITE_HEAD 1
#define READ_HEAD 0
mqd_t mq = 0;
sem_t *sem;
// sem_t *sem_ten;
int prio = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
char *generate_string()
{
    srand(getpid());
    char *new_string = (char *)malloc(sizeof(char) * 10);
    int i = 0;
    for (i = 0; i < 10; i++)
    {
        new_string[i] = rand() % 57 + 64;
    }
    new_string[i] = '\0';
    return new_string;
}
// void *routine(void *args)
// {
//     // int pipe[2];
//     int *pipe = (int *)args;
//     // pipe[0] = ((int **)args)[0];
//     // pipe[1] = ((int **)args)[1];

//     char *buffer = generate_string();
//     sem_wait(sem);

//     pthread_mutex_lock(&mutex);
//     write(pipe[WRITE_HEAD], buffer, strlen(buffer));
//     pthread_mutex_unlock(&mutex);
// }
void child_routine(int pipe__[2])
{
    close(pipe__[READ_HEAD]);
    // pthread_t tid;
    // char *buffer = generate_string();
    char buffer[BUFSIZ];
    strcpy(buffer, "sabin");
    // pthread_create(&tid, NULL, routine, (void *)&pipe__);
    printf("get::%s\n", buffer);
    int sem_value;
    if (sem_getvalue(sem, &sem_value) != 0)
    {
        perror("sem_getvalue failed");
        exit(EXIT_FAILURE);
    }
    printf("Semafor  %d\n", sem_value);
    sem_wait(sem);
    printf("Send::%s\n", buffer);
    // mq_send(mq, buffer, strlen(buffer), 0);
    pthread_mutex_lock(&mutex);
    write(pipe__[WRITE_HEAD], buffer, strlen(buffer));
    pthread_mutex_unlock(&mutex);
}
int main()
{
    sem_t *sem = sem_open(SEM_NAME, O_CREAT | O_RDWR, 0666, 1);
    if (sem == SEM_FAILED)
    {
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }

    // Verificăm valoarea semaforului
    int sem_value;
    if (sem_getvalue(sem, &sem_value) != 0)
    {
        perror("sem_getvalue failed");
        exit(EXIT_FAILURE);
    }
    printf("Semafor valoare inițială: %d\n", sem_value);
    // mq = mq_open(MQ_NAME, O_CREAT | O_RDWR, 0666, NULL);
    int pipe__[2];
    pipe(pipe__);
    // for (int i = 0; i < 10; i++)
    //{
    pid_t pid = fork();
    switch (pid)
    {
    case 0:
    {
        printf("(999)");
        child_routine(pipe__);
        exit(0);
    }
    case -1:
    {
        perror("error at fork");
        exit(EXIT_FAILURE);
    }
    default:
    {
        sleep(100);
        // Codul procesului părinte
        close(pipe__[WRITE_HEAD]); // Închidem capătul de scriere al pipe-ului în procesul părinte

        char buffer[BUFSIZ];
        int i = 0;
        int sem_value;

        if (sem_getvalue(sem, &sem_value) != 0)
        {
            perror("sem_getvalue failed");
            exit(EXIT_FAILURE);
        }
        // sleep(2);
        printf("Val:%d..\n", sem_value);
        if (sem_value == 0)
        {
            // sem_post(sem);
            read(pipe__[READ_HEAD], buffer, sizeof(buffer));
            sem_post(sem);
            // mq_receive(mq, buffer, sizeof(buffer), 0);
            printf("Received::%d::%s\n", i, buffer);
            i++;
        }
    }
    }
    //}
    // close(pipe__[WRITE_HEAD]);
    // for (int i = 0; i < 2; i++)
    // char buffer[BUFSIZ];
    // int i = 0;
    // int sem_value;
    // // while (1)
    // // {
    // sleep(1);
    // if (sem_getvalue(sem, &sem_value) != 0)
    // {
    //     perror("sem_getvalue failed");
    //     exit(EXIT_FAILURE);
    // }
    // // sleep(2);
    // printf("Val:%d..\n", sem_value);
    // if (sem_value == 0)
    // {
    //     // sem_post(sem);
    //     read(pipe__[READ_HEAD], buffer, sizeof(buffer));
    //     sem_post(sem);
    //     // mq_receive(mq, buffer, sizeof(buffer), 0);
    //     printf("Received::%d::%s\n", i, buffer);
    //     i++;
    // }
    // }
    return 0;
}

// #include <stdio.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <pthread.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/types.h>
// #include <time.h>
// #include <sys/wait.h>
// #include <mqueue.h>
// #define MQ_NAME "/mq_name"
// #define READ_HEAD 0
// #define WRITE_HEAD 1
// mqd_t mq;
// void child_handler()
// {
//     // write
//     // close(pipe[READ_HEAD]);
//     char buffer[BUFSIZ];
//     strcpy(buffer, "MESAJJJJ");
//     mq_send(mq, buffer, strlen(buffer), 0);
// }
// int main()
// {

//     mq = mq_open(MQ_NAME, O_CREAT | O_RDWR, 0666);
//     // int pipe__[2];
//     // pipe(pipe__);
//     for (int i = 0; i < 10; i++)
//     {
//         pid_t pid = fork();
//         switch (pid)
//         {
//         case 0:
//         {
//             child_handler();
//             exit(0);
//         }
//         case -1:
//         {
//             exit(EXIT_FAILURE);
//         }
//         default:
//         {
//         }
//         }
//     }
//     while (1)
//     {
//         char buffer[BUFSIZ];
//         mq_receive(mq, buffer, sizeof(buffer), 0);
//         printf("RECV::::%s...\n", buffer);
//     }
//     // close(pipe__[WRITE_HEAD]);
//     // recv
//     mq_unlink(MQ_NAME);
//     return 0;
// }
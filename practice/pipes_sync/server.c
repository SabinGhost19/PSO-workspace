// #include <stdio.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <pthread.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/types.h>
// #include <time.h>
// #include <sys/wait.h>

// #define READ_HEAD 0
// #define WRITE_HEAD 1
// pthread_mutex_t writePipe_mutex_ = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t Pipe_cond = PTHREAD_COND_INITIALIZER;
// int poate_parintele_citit = 0;
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
// void child_routine(int pipe[2])
// {
//     char *new_string = generate_string();
//     printf("This is the string: %s...\n", new_string);
//     pthread_mutex_lock(&writePipe_mutex_);
//     // while (poate_parintele_citit != 1)
//     //{
//     // pthread_cond_wait(&Pipe_cond, &writePipe_mutex_);
//     // }
//     write(pipe[WRITE_HEAD], new_string, strlen(new_string));
//     // poate_parintele_citit = 1;
//     pthread_mutex_unlock(&writePipe_mutex_);
// }
// int main()
// {
//     srand(time(NULL));
//     printf("..............\n");
//     int pipe_com[2];
//     int rc = pipe(pipe_com);
//     if (rc < 0)
//     {
//         perror("error at creating pipe");
//         exit(EXIT_FAILURE);
//     }
//     // close(pipe_com[WRITE_HEAD]);

//     for (int i = 0; i < 10; i++)
//     {
//         pid_t pid = fork();
//         switch (pid)
//         {
//         case 0:
//         {
//             // child
//             close(pipe_com[READ_HEAD]);
//             child_routine(pipe_com);
//             exit(0);
//         }
//         case -1:
//         {
//             //
//             perror("error at fork");
//             exit(EXIT_FAILURE);
//         }
//         default:
//         {
//             // let it empty
//         }
//         }
//     }
//     // sleep(5);
//     char buffer[BUFSIZ];
//     while (1)
//     {
//         // pthread_cond_signal(&Pipe_cond);
//         read(pipe_com[READ_HEAD], buffer, sizeof(buffer));
//         printf("Recv: %s...\n", buffer);
//         // pthread_mutex_lock(&writePipe_mutex_);
//         //  poate_parintele_citit = 1;
//         // pthread_mutex_unlock(&writePipe_mutex_);
//     }

//     int stat = 0;
//     for (int i = 0; i < 2; i++)
//     {
//         wait(&stat);
//         printf("Proc child stat..%d..\n", stat);
//     }
//     return 0;
// }

// #define READ_HEAD 0
// #define WRITE_HEAD 1
// #include <time.h>
// #include <sys/wait.h>
// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <pthread.h>
// #include <errno.h>
// #include <semaphore.h>
// pthread_mutex_t writePipe_mutex_ = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t Pipe_cond = PTHREAD_COND_INITIALIZER;
// int poate_parintele_citit = 0;

// char *generate_string()
// {
//     srand(getpid()); // Folosirea PID-ului pentru a genera un seed unic pentru fiecare proces

//     char *new_string = (char *)malloc(sizeof(char) * 10);
//     int i = 0;
//     for (i = 0; i < 10; i++)
//     {
//         new_string[i] = rand() % 57 + 64; // Generăm caractere între 'A' și 'Z'
//     }
//     new_string[i] = '\0'; // Adăugăm terminatorul de șir
//     return new_string;
// }

// sem_t sem;
// void child_routine(int pipe[2])
// {
//     char *new_string = generate_string();
//     printf("This is the string: %s...\n", new_string);

//     // Așteaptă semnalul din partea părintelui înainte de a scrie în pipe
//     pthread_mutex_lock(&writePipe_mutex_);
//     pthread_cond_wait(&Pipe_cond, &writePipe_mutex_); // Așteaptă semnalul de la părinte
//     // printf("After wait...\n");
//     // Scrierea în pipe

//     write(pipe[WRITE_HEAD], new_string, strlen(new_string));
//     free(new_string); // Eliberăm memoria alocată pentru șir

//     // Permite părintelui să citească din pipe
//     pthread_mutex_unlock(&writePipe_mutex_);
// }
// int main()
// {
//     srand(time(NULL)); // Inițializare random pentru părinte
//     // sem_init(&sem, 1, 0);
//     printf("..............\n");
//     int pipe_com[2];
//     int rc = pipe(pipe_com);
//     if (rc < 0)
//     {
//         perror("Error at creating pipe");
//         exit(EXIT_FAILURE);
//     }

//     // Crearea proceselor copil
//     for (int i = 0; i < 10; i++)
//     {
//         pid_t pid = fork();
//         switch (pid)
//         {
//         case 0:
//         {
//             // Procesul copil
//             close(pipe_com[READ_HEAD]); // Închidem capătul de citire
//             // child_routine(pipe_com);

//             char *new_string = generate_string();
//             printf("This is the string: %s...\n", new_string);

//             // Așteaptă semnalul din partea părintelui înainte de a scrie în pipe
//             pthread_mutex_lock(&writePipe_mutex_);
//             pthread_cond_wait(&Pipe_cond, &writePipe_mutex_); // Așteaptă semnalul de la părinte
//             // printf("After wait...\n");
//             // Scrierea în pipe

//             write(pipe_com[WRITE_HEAD], new_string, strlen(new_string));
//             free(new_string); // Eliberăm memoria alocată pentru șir

//             // Permite părintelui să citească din pipe
//             pthread_mutex_unlock(&writePipe_mutex_);
//             exit(0);
//         }
//         case -1:
//         {
//             perror("Error at fork");
//             exit(EXIT_FAILURE);
//         }
//         default:
//             break;
//         }
//     }

//     // Părintele va aștepta semnalul pentru a citi din pipe
//     char buffer[BUFSIZ];
//     close(pipe_com[WRITE_HEAD]); // Închidem capătul de scriere al pipe-ului

//     // Sincronizarea între părinte și copii
//     // pthread_mutex_lock(&writePipe_mutex_);
//     pthread_cond_signal(&Pipe_cond);

//     for (int i = 0; i < 10; i++)
//     {
//         // Semnalizăm procesul copil să poată începe să scrie în pipe
//         // pthread_cond_signal(&Pipe_cond);
//         // sleep(1); // Poți înlocui cu un alt mecanism de sincronizare mai robust

//         // Citirea din pipe
//         int bytes_read = read(pipe_com[READ_HEAD], buffer, sizeof(buffer) - 1);
//         if (bytes_read < 0)
//         {
//             perror("Error while reading from pipe");
//             exit(EXIT_FAILURE);
//         }
//         buffer[bytes_read] = '\0'; // Asigură-te că șirul citit este terminat corect

//         pthread_cond_signal(&Pipe_cond);
//         printf("Recv: %s...\n", buffer);
//     }
//     // pthread_mutex_unlock(&writePipe_mutex_);

//     // Așteptăm terminarea proceselor copil
//     int status = 0;
//     for (int i = 0; i < 2; i++)
//     {
//         wait(&status);
//         printf("Child process status: %d\n", status);
//     }

//     return 0;
// }

#include <time.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define READ_HEAD 0
#define WRITE_HEAD 1
#define NUM_CHILDREN 10
#define BUF_SIZE 1024

pthread_mutex_t writePipe_mutex_ = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t Pipe_cond = PTHREAD_COND_INITIALIZER;
int poate_parintele_citit = 0;

char *generate_string()
{
    srand(getpid()); // Folosirea PID-ului pentru a genera un seed unic pentru fiecare proces

    char *new_string = (char *)malloc(sizeof(char) * 10);
    int i = 0;
    for (i = 0; i < 10; i++)
    {
        new_string[i] = rand() % 57 + 64; // Generăm caractere între 'A' și 'Z'
    }
    new_string[i] = '\0'; // Adăugăm terminatorul de șir
    return new_string;
}

void child_routine(int pipe[2])
{
    char *new_string = generate_string();
    printf("This is the string: %s...\n", new_string);
    sleep(1);
    write(pipe[WRITE_HEAD], new_string, strlen(new_string));
    free(new_string);
}

int main()
{
    srand(time(NULL));

    printf("..............\n");
    int pipe_com[2];
    int rc = pipe(pipe_com);
    if (rc < 0)
    {
        perror("Error at creating pipe");
        exit(EXIT_FAILURE);
    }

    // Crearea proceselor copil
    for (int i = 0; i < NUM_CHILDREN; i++)
    {
        pid_t pid = fork();
        switch (pid)
        {
        case 0:
        {
            // Procesul copil
            close(pipe_com[READ_HEAD]); // Închidem capătul de citire
            child_routine(pipe_com);
            exit(0);
        }
        case -1:
        {
            perror("Error at fork");
            exit(EXIT_FAILURE);
        }
        default:
            break;
        }
    }

    // Părintele va aștepta semnalul pentru a citi din pipe
    char buffer[BUF_SIZE];
    close(pipe_com[WRITE_HEAD]); // Închidem capătul de scriere al pipe-ului

    // Sincronizarea între părinte și copii
    for (int i = 0; i < NUM_CHILDREN; i++)
    {

        int bytes_read = read(pipe_com[READ_HEAD], buffer, sizeof(buffer) - 1);
        if (bytes_read < 0)
        {
            perror("Error while reading from pipe");
            exit(EXIT_FAILURE);
        }
        buffer[bytes_read] = '\0';

        printf("Recv: %s...\n", buffer);
    }

    // Așteptăm terminarea proceselor copil
    int status = 0;
    for (int i = 0; i < NUM_CHILDREN; i++)
    {
        wait(&status);
        printf("Child process status: %d\n", status);
    }

    return 0;
}
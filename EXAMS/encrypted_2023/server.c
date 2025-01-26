#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <openssl/rand.h>
#include <string.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define READ_HEAD 0
#define WRITE_HEAD 1
#define FIRST_PIPE 0
#define SECOND_PIPE 1
char key[40];
char filename[100];
int NR_PROC = 0;
void init(int argc, char *argv[]);
typedef struct PIP
{
    int read_child_pipe[2];
    int write_child_pipe[2];
} PIP;
sem_t sem;
void generate_random_key()
{
    ssize_t size_key = 16;
    unsigned char buffer[size_key];

    if (RAND_bytes(buffer, size_key) != 1)
    {
        perror("Error generating random bytes");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < size_key; i++)
    {
        sprintf(key + (i * 2), "%02x", buffer[i]);
    }
}
void read_from_file(char *buffer, char *filename);
void child_handle_functin(PIP pip)
{

    if (key == NULL || strlen(key) != 32)
    {
        perror("Invalid key");
        exit(EXIT_FAILURE);
    }

    char *exec_args[] = {"/usr/bin/openssl", "enc", "-aes-128-ecb", "-e", "-K", key, "-nopad", "-out", "output.txt", NULL};

    // int fd = open("file_out.txt", O_RDWR | O_CREAT, 0644);
    // if (fd == -1)
    // {
    //     perror("Eroare la deschiderea fișierului");
    //     exit(EXIT_FAILURE);
    // }

    // if (dup2(fd, STDOUT_FILENO) == -1)
    // {
    //     perror("Eroare la redirecționarea stdout");
    //     exit(EXIT_FAILURE);
    // }

    if (dup2(pip.read_child_pipe[READ_HEAD], STDIN_FILENO) == -1)
    {
        perror("dup2 failed for stdin");
        exit(EXIT_FAILURE);
    }

    close(pip.read_child_pipe[READ_HEAD]);
    // close(fd);

    int err = execvp(exec_args[0], exec_args);
    if (err < 0)
    {
        perror("EROARE LA LANSAREA EXECUTABILULUI");
        exit(EXIT_FAILURE);
    }
}
PIP init_pip_for_CHILDERN(int pipe_fd[2][NR_PROC][2], int index)
{
    PIP new_pip;
    new_pip.read_child_pipe[READ_HEAD] = pipe_fd[index][FIRST_PIPE][READ_HEAD];
    new_pip.read_child_pipe[WRITE_HEAD] = pipe_fd[index][FIRST_PIPE][WRITE_HEAD];
    close(new_pip.read_child_pipe[WRITE_HEAD]);

    new_pip.write_child_pipe[READ_HEAD] = pipe_fd[index][SECOND_PIPE][READ_HEAD];
    new_pip.write_child_pipe[WRITE_HEAD] = pipe_fd[index][SECOND_PIPE][WRITE_HEAD];
    close(new_pip.write_child_pipe[READ_HEAD]);

    return new_pip;
}
void *starting_thread_routine(void *args)
{
    // copy the pipes WRITE HEADS
    int pipe[2];
    pipe[READ_HEAD] = ((int *)args)[READ_HEAD];
    pipe[WRITE_HEAD] = ((int *)args)[WRITE_HEAD];

    char buffer[1024];
    read_from_file(buffer, filename);
    printf("Readed from file:%s\n\n", buffer);
    // write to childs
    int rc = write(pipe[WRITE_HEAD], buffer, (strlen(buffer) + 1) * sizeof(char));
}
int main(int argc, char *argv[])
{
    init(argc, argv);
    generate_random_key();
    printf("Result:%s\n", key);

    NR_PROC = atoi(argv[2]);
    strcpy(filename, argv[1]);
    // ex: 2 proc, 2 pipes and 2 heads for eatch
    int pipe_fd[NR_PROC][2][2];
    //
    //
    // creating pipes

    int val = 0;
    for (int i = 0; i < NR_PROC; i++)
    {
        val = pipe(pipe_fd[i][FIRST_PIPE]);
        if (val < 0)
        {
            perror("error on creating pipe");
            exit(-1);
        }
        val = pipe(pipe_fd[i][SECOND_PIPE]);
        if (val < 0)
        {
            perror("error on creating pipe");
            exit(-1);
        }
    }
    for (int i = 0; i < NR_PROC; i++)
    {
        pid_t pid = fork();
        switch (pid)
        {
        case 0:
        {

            PIP new_pip = init_pip_for_CHILDERN(pipe_fd, i);
            child_handle_functin(new_pip);
            exit(0);
            break;
        }
        case -1:
        {
            perror("Fork error");
            exit(EXIT_FAILURE);
            break;
        }
        default:
        {
            printf("This is the parent process with child PID: %d\n", pid);
            break;
        }
        }
    }
    // close pipe heads:
    for (int i = 0; i < NR_PROC; i++)
    {
        // in first we write
        // in second we read
        // parent perspective
        close(pipe_fd[i][FIRST_PIPE][READ_HEAD]);
        close(pipe_fd[i][SECOND_PIPE][WRITE_HEAD]);
    }
    // create_pthreads:
    pthread_t tid[NR_PROC];
    int values[NR_PROC];
    for (int i = 0; i < NR_PROC; i++)
    {
        values[i] = i;
    }
    for (int i = 0; i < NR_PROC; i++)
    {
        if (pthread_create(&tid[i], NULL, &starting_thread_routine, (void *)pipe_fd[i][FIRST_PIPE]) != 0)
        {
            perror("error at creating thread");
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < NR_PROC; i++)
    {
        if (pthread_join(tid[i], NULL) != 0)
        {
            perror("error at joining thread");
            exit(EXIT_FAILURE);
        }
    }
    sleep(3);
    return 0;
}
void read_from_file(char *buffer, char *filename)
{
    ssize_t size_to_read = 16;
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        perror("open file");
        exit(EXIT_FAILURE);
    }
    int read_btyes = read(fd, buffer, size_to_read * NR_PROC * sizeof(char));
    if (read_btyes <= 0)
    {
        perror("reading error from file");
        exit(EXIT_FAILURE);
    }
}

void init(int argc, char *argv[])
{
    if (argc < 3)
    {
        perror("To few args");
        exit(EXIT_FAILURE);
    }
    printf("First: %s\n", argv[1]);
    printf("Second: %s\n", argv[2]);
    printf("Third: %s\n", argv[3]);
}

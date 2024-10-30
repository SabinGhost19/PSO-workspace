#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define PIPE_NAME "myPIPE"
#define BUFFER_SIZE 4

void initialize_named_pipe()
{
    pid_t res_status = mkfifo(PIPE_NAME, 0666);

    if (res_status < 0)
    {
        perror("Eroare la crearea pipe-ului");
        exit(-1);
    }
}
void sig_usr_handle()
{
    printf("Sig usr received in the parrent proc\n");

    int fd = open(PIPE_NAME, O_WRONLY);
    if (fd < 0)
    {
        perror("Error at opening named pipe for writing");
        exit(EXIT_FAILURE);
    }

    char *buffer = "000000000000";

    ssize_t write_bytes = write(fd, buffer, strlen(buffer));
    if (write_bytes < 0)
    {
        perror("Error at writing into named pipe");
        exit(EXIT_FAILURE);
    }

    close(fd);
}
int main()
{
    int children_pid_vector[10];

    int nr_of_child_procceses;
    printf("Introdu numarul de procese: ");
    scanf("%d", &nr_of_child_procceses);

    initialize_named_pipe();
    printf("Waiting for signal: %ld", (long)getpid());
    fflush(stdout); //  golirea bufferului
    // nu imi afisa

    pid_t Child_pid = 0;
    for (int i = 0; i < nr_of_child_procceses; i++)
    {
        switch (Child_pid = fork())
        {
        case 0:
        {

            int fd = open(PIPE_NAME, O_RDONLY);
            if (fd < 0)
            {
                perror("Error at opening named pipe for reading");
                exit(EXIT_FAILURE);
            }

            char buffer[BUFFER_SIZE];
            ssize_t bytes_read = read(fd, buffer, BUFFER_SIZE);
            if (bytes_read < 0)
            {
                perror("Error at reading bytes from named pipe");
                exit(EXIT_FAILURE);
            }

            buffer[bytes_read] = '\0';
            printf("Bytes read by the CHILD proc %ld: %s\n", (long)getpid(), buffer);

            close(fd);
            exit(0);
        }

        case -1:
        {

            perror("Eroare la crearea proceselor: fork()");
            exit(EXIT_FAILURE);
        }

        default:
        {
            // memorare pid copil
            children_pid_vector[i] = Child_pid;
        }
        }
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sig_usr_handle;

    // asignare sigaction(handler deocamdata) semnalului
    sigaction(SIGUSR1, &sa, NULL);
    printf("Waiting for signal: %ld", (long)getpid());

    // asteptam semnalul
    pause();

    wait(NULL);

    // for (int i = 0; i < nr_of_child_procceses; i++)
    // {
    //     wait(NULL);
    //     waitpid(children_pid_vector[i], NULL, 0);
    // }
    unlink(PIPE_NAME);

    return 0;
}

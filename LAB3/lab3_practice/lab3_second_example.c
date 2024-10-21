#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 17
#define READ_END 0
#define WRITE_END 1

int main()
{
    int pipe_fd[2];

    int ret = pipe(pipe_fd);
    if (ret < 0)
    {
        perror("eroare on creating pipe: ");
        exit(-1);
    }

    char buffer[BUFFER_SIZE];
    pid_t pid;
    pid = fork();
    switch (pid)
    {
    case 0:
        close(pipe_fd[WRITE_END]);

        int ret = read(pipe_fd[READ_END], buffer, BUFFER_SIZE);

        buffer[ret] = '\0';

        printf("procesul fiu: %s", buffer);

        close(pipe_fd[READ_END]);

        exit(0);

        break;
    case -1:
        break;
    default:
        strcpy(buffer, "BUna din parinte");

        close(pipe_fd[READ_END]);

        write(pipe_fd[WRITE_END], buffer, strlen(buffer));

        close(pipe_fd[WRITE_END]);

        wait(NULL);

        exit(0);
        break;
    }

    return 0;
}
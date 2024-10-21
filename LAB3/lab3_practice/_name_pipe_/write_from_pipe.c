#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define pathName "my_pipe"

int main()
{

    char *buffer = "Acesta se scrie in pipe prin write";
    pid_t pipe_fd = open(pathName, O_WRONLY);
    if (pipe_fd < 0)
    {
        perror("error opening pipe file");
        exit(-1);
    }

    ssize_t write_bytes = write(pipe_fd, buffer, strlen(buffer));
    if (write_bytes < 0)
    {
        perror("error writing in pipe file");
        exit(-1);
    }

    close(pipe_fd);

    return 0;
}
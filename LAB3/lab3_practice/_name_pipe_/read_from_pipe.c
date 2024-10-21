#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define pathName "my_pipe"
#define BUFFER_SIZE 100
int main()
{
    pid_t pipe_pid = open(pathName, O_RDONLY);
    if (pipe_pid < 0)
    {
        perror("error opening pipe");
        exit(-1);
    }

    char buffer[BUFFER_SIZE];
    ssize_t read_bytes = read(pipe_pid, buffer, BUFFER_SIZE);
    if (read_bytes < 0)
    {
        perror("read from pipe");
        exit(-1);
    }
    buffer[read_bytes] = '\0';

    printf("Bytes read from pipe: %s", buffer);
    close(pipe_pid);

    return 0;
}
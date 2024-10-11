#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{

    ssize_t fd = open("fisier3.txt", O_WRONLY | O_RDONLY | O_CREAT, 0600);
    if (fd < 0)
    {
        perror("Eroare la deschiderea fisierului");
        close(fd);
        exit(-1);
    }

    ssize_t write_bytes = write(fd, "sabin", 5);
    if (write_bytes < 0)
    {
        perror("Eroare la scrierea in fisier");
        close(fd);
        exit(-1);
    }

    int stdout_copy = dup(1);
    if (dup2(fd, 1) < 0)
    {
        perror("Eroare restaurare stdout");
        close(fd);
        exit(-1);
    }

    dup2(stdout_copy, 1);
    write_bytes = write(1, "ana", 3);

    close(fd);

    return 0;
}
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *arhv[])
{
    char buffer[100];
    ssize_t fd = open("fisier2.txt", O_RDONLY | O_CREAT, 0600);
    if (fd < 0)
    {
        perror("Eroare deschidere fisier");
        close(fd);
        exit(-1);
    }
    ssize_t read_bytes = read(fd, buffer, 100);
    if (read_bytes < 0)
    {
        perror("Eroare la citire din fisier");
    }
    ssize_t write_bytes = write(1, buffer, strlen(buffer) * sizeof(char));
    if (write_bytes < 0)
    {
        perror("Eroare la scrierea in fisier");
        close(fd);
        exit(-1);
    }

    if (close(fd) == -1)
    {
        perror("Failed to close file");
        exit(-1);
    }

    //---dup and dup2 practice---
    fd = open("output.txt", O_RDWR | O_CREAT | O_TRUNC, 0600);

    int stdout_copy = dup(1);
    if (stdout_copy < 0)
    {
        perror("Eroare salvare stdout");
        close(fd);
        exit(-1);
    }

    if (dup2(fd, 1) < 0)
    {
        perror("Eroare restaurare stdout");
        close(stdout_copy);
        exit(-1);
    }

    write_bytes = write(fd, "sas", 3);
    if (write_bytes < 0)
    {
        perror("Eroare scriere fisier");
        close(fd);
        exit(-1);
    }

    if (dup2(stdout_copy, 1) < 0)
    {
        perror("Eroare restaurare stdout");
        close(stdout_copy);
        exit(-1);
    }

    write_bytes = write(1, "sas", 3);
    if (write_bytes < 0)
    {
        perror("Eroare scriere fisier");
        close(fd);
        exit(-1);
    }

    return 0;
}
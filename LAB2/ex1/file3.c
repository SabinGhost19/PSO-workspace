#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int funct(int n)
{
    int c = 0;
    int d = 0;
    while (n > 0)
    {
        d += 1;
        n = n - 2;
        c = c + n - 2;
    }

    return c;
}
int main(int argc, char *argv[])
{

    int c = funct(120);

    printf("NUmber: %i\n", c);
    int d = funct(250);

    printf("NUmber2: %i\n", d);

    c = funct(340);

    printf("NUmber: %i\n", c);
    c = funct(480);

    printf("NUmber: %i\n", c);
    c = funct(700);

    printf("NUmber: %i\n", c);
    // ssize_t fd = open("fisier3.txt", O_WRONLY | O_RDONLY | O_CREAT, 0600);
    // if (fd < 0)
    // {
    //     perror("Eroare la deschiderea fisierului");
    //     close(fd);
    //     exit(-1);
    // }

    // ssize_t write_bytes = write(fd, "sabin", 5);
    // if (write_bytes < 0)
    // {
    //     perror("Eroare la scrierea in fisier");
    //     close(fd);
    //     exit(-1);
    // }

    // int stdout_copy = dup(1);
    // if (dup2(fd, 1) < 0)
    // {
    //     perror("Eroare restaurare stdout");
    //     close(fd);
    //     exit(-1);
    // }

    // dup2(stdout_copy, 1);
    // write_bytes = write(1, "ana", 3);

    // close(fd);

    return 0;
}
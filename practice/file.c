#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    // stergere cu unlink
    char *buffer = "Acesta este un fisier custom";
    char *info_message = "Fisierul a fost suprascris\n";
    off_t offset;

    size_t fd = open("fisier.txt", O_WRONLY | O_TRUNC | O_CREAT, 0640);
    if (fd < 0)
    {
        perror("Eroare deschidere fisier");
        close(fd);
        exit(-1);
    }
    int write_bytes = write(fd, buffer, strlen(buffer));

    if (write_bytes < 0)
    {
        perror("Eror opening writing in file");
        close(fd);
        exit(-1);
    }

    // sciere in fis 1
    write_bytes = 0;
    write_bytes = write(STDOUT_FILENO, info_message, strlen(info_message));
    if (write_bytes == -1)
    {
        perror("Eroare scriere in fisierul 1");
        close(fd);
        exit(-1);
    }

    //---offsetting and seek ---
    offset = lseek(fd, 0, SEEK_END);
    write_bytes = 0;
    write_bytes = write(fd, "SSS", 3);
    if (write_bytes < 0)
    {
        perror("Eraore la scrierea in fisier");
        close(fd);
        exit(-1);
    }

    close(fd);

    return 0;
}
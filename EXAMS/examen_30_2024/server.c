#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

#define SHM_NAME "/my_shared_memory"

int main()
{

    // Creăm un obiect de memorie partajată
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("shm_open");
        return 1;
    }

    // Setăm dimensiunea zonei de memorie partajată
    if (ftruncate(shm_fd, sizeof(char) * 1024) == -1)
    {
        perror("ftruncate");
        return 1;
    }

    int fd = open("file.txt", O_RDWR);

    // Mapăm memoria în spațiul virtual al procesului
    char *data[10];

    char *data[0] = mmap(NULL, sizeof(char) * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (data == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    ((int *)data)[0] = 1;
    data[4] = 'S';
    // Scriem în memoria partajată
    // snprintf(data, 1024, "Acesta este un ată!");

    // printf("Date scrise in memoria partajata: %s\n", data);

    // Dezmapăm și închidem
    if (munmap(data, sizeof(char) * 1024) == -1)
    {
        perror("munmap");
    }
    close(fd);
    close(shm_fd);

    // stergere memorie partajata
    //  if (shm_unlink(SHM_NAME) == -1)
    //  {
    //      perror("shm_unlink");
    //      return 1;
    //  }

    return 0;
}

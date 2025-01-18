// #include <sys/mman.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <stdio.h>
// #include <sys/types.h>

// int main()
// {
//     int fd = open("file.txt", O_RDWR);
//     if (fd == -1)
//     {
//         perror("open");
//         return 1;
//     }

//     // Obținem dimensiunea fișierului
//     off_t file_size = lseek(fd, 0, SEEK_END);

//     // Mapăm fișierul în memorie
//     char *data = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
//     if (data == MAP_FAILED)
//     {
//         perror("mmap");
//         close(fd);
//         return 1;
//     }

//     // Modificăm conținutul fișierului în memorie
//     data[0] = 'H';

//     // Sincronizăm modificările în fișier
//     if (msync(data, file_size, MS_SYNC) == -1)
//     {
//         perror("msync");
//     }

//     // Dezmapăm fișierul
//     if (munmap(data, file_size) == -1)
//     {
//         perror("munmap");
//     }

//     close(fd);
//     return 0;
// }

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

#define SHM_NAME "/my_shared_memory"

int main()
{
    // Deschidem segmentul de memorie partajată
    int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (shm_fd == -1)
    {
        perror("shm_open");
        return 1;
    }

    // Mapăm memoria în spațiul virtual al procesului
    char *data = mmap(NULL, sizeof(char) * 1024, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    // Citim din memoria partajată
    printf("Date citite din memoria partajată: %s\n", data);

    // Dezmapăm și închidem
    if (munmap(data, sizeof(char) * 1024) == -1)
    {
        perror("munmap");
    }
    close(shm_fd);

    return 0;
}

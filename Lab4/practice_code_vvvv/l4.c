#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>

int main()
{
    pid_t pid = 0;
    int fd[2];
    if (pipe(fd) < 0)
    {
        perror("Error at pipe");
        exit(EXIT_FAILURE);
    }
    switch (pid = fork())
    {
    case -1:
    {
        perror("Error at fork");
        exit(EXIT_FAILURE);
    }

    case 0:
    {
        // child
        srand(time(NULL));
        int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        int nr = 10;
        close(fd[0]);
        write(fd[SD], &nr, sizeof(int));
        write(fd[1], arr, sizeof(int) * nr);

        close(fd[1]);
        exit(0);
    }

    default:
    {
        // parent
        close(fd[1]);
        int nr;
        read(fd[0], &nr, sizeof(int));
        int arr[10];
        read(fd[0], arr, sizeof(int) * nr);
        printf("Primirea la parinte\n");
        for (int i = 0; i < 10; i++)
        {
            printf("%d  ", arr[i]);
        }
        wait(NULL);
    }
    }

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

int main()
{

    printf("mmmmdsadas\n");

    int pid = 0;
    switch (pid = fork())
    {
    case 0:
    {

        while (1)
        {
            usleep(500000);
            printf("texttt\n");
        }
    }
    case -1:
        return 1;

    default:
    {
        sleep(2);
        kill(pid, SIGKILL);
        wait(NULL);
    }
    }
    return 0;
}
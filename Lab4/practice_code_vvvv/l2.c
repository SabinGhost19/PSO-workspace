#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

int main()
{
    pid_t pid = 0;
    switch (pid = fork())
    {
    case 0:
    {
        // child
        while (1)
        {
            printf("CHILD...\n");
        }
    }
    case -1:
    {
        return -1;
    }
    default:
    {
        kill(pid, SIGSTOP);
        int nr = 0;
        // parent
        do
        {
            printf("Introduceti cate secunde sa se afiseze mesajul:");
            scanf("%d", &nr);

            if (nr > 0)
            {
                kill(pid, SIGCONT);
                sleep(nr);
                kill(pid, SIGSTOP);
            }
        } while (nr > 0);
    }
    }
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void signal_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        printf("\nSIGUSR1 received. Continuing execution..\n");
    }
}

int main()
{
    printf("Process %d started\n", getpid());

    /*Semnale ca SIGKILL sau SIGSTOP nu pot fi prinse*/
    if (signal(SIGKILL, signal_handler) == SIG_ERR)
    {
        printf("You shall not catch SIGKILL\n");
    }

    if (signal(SIGINT, SIG_IGN) == SIG_ERR)
    {
        printf("Unable to ignore SIGINT\n");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGUSR1, signal_handler) == SIG_ERR)
    {
        printf("Unable to catch SIGUSR1");
        exit(EXIT_FAILURE);
    }

    printf("Press CTRL+C to try and stop me\n");

    while (1)
    {
        sleep(1);
    }

    return 0;
}

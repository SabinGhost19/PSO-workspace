#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

void handler_sigUSR1(int sig)
{
    printf("HINT\n");
}
void handle_TERM(int sig)
{
    printf("CHILD TERM....\n");
    exit(0);
}
int main()
{
    pid_t pid = 0;
    switch (pid = fork())
    {
    case 0:
    {
        struct sigaction sa;
        sa.sa_handler = handle_TERM;
        sa.sa_flags = SA_RESTART;
        sigaction(SIGTERM, &sa, NULL);

        while (1)
        {
            sleep(2);
            kill(getppid(), SIGUSR1);
        }
        exit(0);
    }
    case -1:
    {
        return 1;
    }
    default:
    {

        struct sigaction sa;
        sa.sa_handler = handler_sigUSR1;
        sa.sa_flags = SA_RESTART;
        sigaction(SIGUSR1, &sa, NULL);

        char answer[10];
        printf("Intreabrea.....");
        scanf("%s", answer);
        printf("Answer:...%s\n", answer);

        kill(pid, SIGTERM);
        wait(NULL);
    }
    break;
    }
    return 0;
}
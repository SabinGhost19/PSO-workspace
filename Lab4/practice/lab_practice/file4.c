#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

void handle(int sig_nr, siginfo_t *info, void *context)
{
    printf("tratarea semnalului: %d cu val: %d ....CHILD\n", sig_nr, info->si_value.sival_int);
}
void handle_parent(int sig_nr, siginfo_t *info, void *context)
{
    printf("Semnal : %d cu val %d IN PARENT\n", sig_nr, info->si_value.sival_int);
}
int main()
{

    struct sigaction sa;
    sa.sa_sigaction = handle_parent;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    pid_t pid = 0;
    switch (pid = fork())
    {
    case 0:
    {
        // child
        sa.sa_sigaction = handle;
        sigaction(SIGUSR1, &sa, NULL);

        sigset_t block_set;
        sigfillset(&block_set);
        sigdelset(&block_set, SIGUSR1);
        sigprocmask(SIG_BLOCK, &block_set, NULL);

        // blocate toate semnalele cu execeptia lui SIGINT
        sigsuspend(&block_set);

        printf("Child exiting...\n");
        exit(0);
    }
    case -1:
    {
    }
    default:
    {
        // parent
        union sigval value;
        value.sival_int = 74;

        sleep(3);
        if (sigqueue(pid, SIGUSR1, value) == -1)
        {
            perror("Eroare la trimitera cu sigqueue\n");
            exit(EXIT_FAILURE);
        }
        printf("Signal sent from the parent proc\n");
        wait(NULL);
        printf("Exiting parent proc\n");
    }
    }

    return 0;
}
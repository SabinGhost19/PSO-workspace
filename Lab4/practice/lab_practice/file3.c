#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

void handle_sgnal_USR1(int signal_nr, siginfo_t *info, void *context)
{
    if (signal_nr == SIGUSR1)
    {

        printf("Semnal %d primit cu valoarea: %d\n", signal_nr, info->si_value.sival_int);
    }
}
int main()
{

    pid_t pid = 0;
    switch (pid = fork())
    {
    case 0:
    {

        // child

        struct sigaction sa;
        sa.sa_flags = SA_SIGINFO;
        sigemptyset(&sa.sa_mask);
        sa.sa_sigaction = &handle_sgnal_USR1;
        sigaction(SIGUSR1, &sa, NULL);

        // blocam semnalul SIGUSR1
        sigset_t block_set;
        sigemptyset(&block_set);
        sigaddset(&block_set, SIGUSR1);

        printf("Semanlule este blocat in copil\n");
        sigprocmask(SIG_BLOCK, &block_set, NULL);
        sleep(4);

        printf("Semnalul este deblocat in copil\n");
        sigprocmask(SIG_UNBLOCK, &block_set, NULL);

        printf("Procesul copil a terminat execuția.\n");
        exit(0);
    }
    case -1:
    {
        perror("Error at froking");
        exit(EXIT_FAILURE);
    }
    default:
    {
        // parent
        union sigval value;
        value.sival_int = 45;
        sleep(1);
        if (sigqueue(pid, SIGUSR1, value) == -1)
        {
            perror("Sigenqueue error at sending");
            exit(EXIT_FAILURE);
        }
        printf("Semnal trimis din parinte\n");
        wait(NULL);
        printf("Procesul părinte a terminat execuția.\n");
    }
    }
    return 0;
}
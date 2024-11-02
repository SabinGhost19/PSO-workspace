#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

void handleUSR1(int nr_sig, siginfo_t *info, void *context)
{
    printf("Semnal primit: %d\n", info->si_signo);
    printf("Codul erorii: %d\n", info->si_errno);
    printf("Codul semnalului: %d\n", info->si_code);
    printf("ID-ul procesului emitent: %d\n", info->si_pid);
    printf("UID-ul utilizatorului emitent: %d\n", info->si_uid);
    printf("Valoarea transmisă: %d\n", info->si_value.sival_int);
}
int main()
{
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = &handleUSR1;
    if (sigaction(SIGUSR1, &sa, NULL) == -1)
    {
        perror("Error at conf handler to the USR1 signal");
        exit(EXIT_FAILURE);
    }
    pid_t pid = 0;
    switch (pid = fork())
    {
    case 0:
    {
        // child

        // sleep(3);
        printf("Child pid: %ld", (long)getpid());
        // transmitere si la parinte
        // se mosteneste si struct sigactionn de la parinte la copil
        // asa ca si parintele si copilul vor avea aceasi structura
        //  other words....acelasi handler pentru SIGUSR1
        kill(getppid(), SIGUSR1);

        exit(0);
    }
    case -1:
    {
        perror("Error at forking");
        exit(EXIT_FAILURE);
    }
    default:
    {
        // parent
        // sleep(1);
        union sigval value;
        value.sival_int = 3;
        if (sigqueue(pid, SIGUSR1, value) == -1)
        {
            perror("Eroare la trimiterea semnalului SIGUSR1");
            exit(EXIT_FAILURE);
        }
        // sleep(1);
        // kill(pid, SIGUSR1);
        wait(NULL);
    }
    }
    return 0;
}
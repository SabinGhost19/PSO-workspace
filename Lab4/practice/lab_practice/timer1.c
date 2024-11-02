#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

void timer_handler(int signo, siginfo_t *info, void *context)
{
    printf("Timer expirat! Valoare atasata: %d\n", info->si_value.sival_int);
}

int main()
{
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timer_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGRTMIN, &sa, NULL);

    timer_t timerid;
    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_int = 163;
    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1)
    {
        perror("Eroare la crearea timer-ului");
        exit(EXIT_FAILURE);
    }

    printf("Timer creat cu succes!\n");
    return 0;
}

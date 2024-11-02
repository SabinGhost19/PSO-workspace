#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

void timer_handler(int signo, siginfo_t *info, void *context)
{
    printf("Timer expirat! Valoare atașată: %d\n", info->si_value.sival_int);
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
    sev.sigev_value.sival_int = 142;

    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1)
    {
        perror("Eroare la crearea timer-ului");
        exit(EXIT_FAILURE);
    }

    struct itimerspec its;
    its.it_value.tv_sec = 2;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 2;
    its.it_interval.tv_nsec = 0;

    if (timer_settime(timerid, 0, &its, NULL) == -1)
    {
        perror("Eroare la armarea timer-ului");
        exit(EXIT_FAILURE);
    }

    printf("Timer creat și armat pentru 2 secunde. Așteptăm 5 secunde și apoi ștergem timer-ul...\n");

    sleep(5);

    if (timer_delete(timerid) == -1)
    {
        perror("Eroare la ștergerea timer-ului");
        exit(EXIT_FAILURE);
    }

    printf("Timer șters.\n");

    return 0;
}

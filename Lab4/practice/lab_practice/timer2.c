#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

void handle_timer(int sig_nr, siginfo_t *info, void *context)
{
    printf("Handle timer, timer expirat, Valoarea atasata: %d\n", info->si_value.sival_int);
}

int main()
{
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = handle_timer;
    if (sigaction(SIGRTMIN, &sa, NULL) == -1)
    {
        perror("Error la configurarea handler-ului de semnal");
        exit(EXIT_FAILURE);
    }

    timer_t timer_ID;
    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_int = 43;
    if (timer_create(CLOCK_REALTIME, &sev, &timer_ID) == -1)
    {
        perror("Crearea timerului a esuat...");
        exit(EXIT_FAILURE);
    }

    struct itimerspec its;
    its.it_value.tv_sec = 5;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 2;
    its.it_interval.tv_nsec = 0;

    if (timer_settime(timer_ID, 0, &its, NULL) == -1)
    {
        perror("Eroare la armarea timer-ului");
        exit(EXIT_FAILURE);
    }

    printf("Timer armat pentru 2 secunde.\n");

    while (1)
    {
        pause();
    }

    return 0;
}

#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <limits.h>
#include <stdlib.h>
volatile sig_atomic_t restart_flag = 0;
timer_t timerid;
struct sigevent sev;
struct itimerspec its;
void usr1_handler(int sig)
{
    if (sig == SIGUSR1)
    {
        printf("Received SIGUSR1\n");
        restart_flag = 1;
    }
}
void term_handler(int sig)
{
    printf("Timer EXPIRED!!!!...exiting...\n");
    exit(0);
}
void timer_setup()
{
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGTERM;
    sev.sigev_value.sival_ptr = &timerid;
    timer_create(CLOCK_REALTIME, &sev, &timerid);

    its.it_value.tv_sec = 10;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;

    timer_settime(timerid, 0, &its, NULL);
}
void restart_timer()
{
    its.it_value.tv_sec = 10;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;

    timer_settime(timerid, 0, &its, NULL);
}
void child_proc()
{
    while (1)
    {
        sleep(5);
        kill(getppid(), SIGUSR1);
    }
}
int main()
{
    pid_t pid = fork();
    if (pid == 0)
    {
        child_proc();
        exit(0);
    }
    char real[100];
    // realpath("server.c", real);
    // printf("Base: %s \n", real);
    struct sigaction sa_usr1;
    memset(&sa_usr1, 0, sizeof(sa_usr1));
    sa_usr1.sa_handler = usr1_handler;
    sigaction(SIGUSR1, &sa_usr1, NULL);

    struct sigaction sa_term;
    memset(&sa_term, 0, sizeof(sa_term));
    sa_term.sa_handler = term_handler;
    sigaction(SIGTERM, &sa_term, NULL);

    timer_setup();
    printf("Timer started...PID: %d\n", getpid());
    while (1)
    {
        if (restart_flag == 1)
        {
            printf("Semnal SIGUSR1 received\n");
            restart_timer();
            restart_flag = 0;
        }
    }
    wait(NULL);
    timer_delete(timerid);
    return 0;
}
// #define _POSIX_C_SOURCE 199309L
// #include <stdio.h>
// #include <stdlib.h>
// #include <signal.h>
// #include <unistd.h>
// #include <time.h>

// timer_t timerid;                       // ID-ul timerului
// volatile sig_atomic_t reset_timer = 0; // Flag pentru resetarea timerului

// // Funcția de tratare a semnalului SIGUSR1
// void handle_sigusr1(int signum)
// {
//     if (signum == SIGUSR1)
//     {
//         reset_timer = 1; // Setează flagul pentru resetarea timerului
//     }
// }

// // Funcția de tratare a semnalului de expirare a timerului
// void handle_timer(int signum, siginfo_t *info, void *context)
// {
//     if (signum == SIGALRM)
//     {
//         printf("Timerul a expirat. Iesire din program...\n");
//         exit(EXIT_SUCCESS);
//     }
// }

// // Funcția pentru configurarea timerului
// void setup_timer()
// {
//     struct sigevent sev;
//     struct itimerspec its;

//     // Configurează evenimentul pentru timer
//     sev.sigev_notify = SIGEV_SIGNAL;
//     sev.sigev_signo = SIGALRM;
//     sev.sigev_value.sival_ptr = &timerid;
//     if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1)
//     {
//         perror("timer_create");
//         exit(EXIT_FAILURE);
//     }

//     // Configurează timerul să expire după 5 secunde
//     its.it_value.tv_sec = 10;
//     its.it_value.tv_nsec = 0;
//     its.it_interval.tv_sec = 0;
//     its.it_interval.tv_nsec = 0;

//     if (timer_settime(timerid, 0, &its, NULL) == -1)
//     {
//         perror("timer_settime");
//         exit(EXIT_FAILURE);
//     }
// }

// // Funcția pentru resetarea timerului
// void reset_timer_func()
// {
//     struct itimerspec its;

//     // Reconfigurează timerul să expire după 5 secunde
//     its.it_value.tv_sec = 10;
//     its.it_value.tv_nsec = 0;
//     its.it_interval.tv_sec = 0;
//     its.it_interval.tv_nsec = 0;

//     if (timer_settime(timerid, 0, &its, NULL) == -1)
//     {
//         perror("timer_settime");
//         exit(EXIT_FAILURE);
//     }
// }

// int main()
// {
//     struct sigaction sa_usr1, sa_timer;

//     // Configurează tratarea semnalului SIGUSR1
//     sa_usr1.sa_handler = handle_sigusr1;
//     sigemptyset(&sa_usr1.sa_mask);
//     sa_usr1.sa_flags = 0;
//     if (sigaction(SIGUSR1, &sa_usr1, NULL) == -1)
//     {
//         perror("sigaction SIGUSR1");
//         exit(EXIT_FAILURE);
//     }

//     // Configurează tratarea semnalului SIGALRM (expirare timer)
//     sa_timer.sa_sigaction = handle_timer;
//     sigemptyset(&sa_timer.sa_mask);
//     sa_timer.sa_flags = SA_SIGINFO;
//     if (sigaction(SIGALRM, &sa_timer, NULL) == -1)
//     {
//         perror("sigaction SIGALRM");
//         exit(EXIT_FAILURE);
//     }

//     // Configurează timerul
//     setup_timer();

//     printf("Timerul a fost pornit. Aștept semnal SIGUSR1 pentru resetare...\n");
//     printf("PID: %d...\n", getpid());
//     while (1)
//     {
//         if (reset_timer)
//         {
//             printf("Semnal SIGUSR1 primit. Resetare timer...\n");
//             reset_timer_func();
//             reset_timer = 0; // Resetează flagul
//         }
//         sleep(1); // Așteaptă 1 secundă pentru a evita consumul excesiv de CPU
//     }

//     return 0;
// }
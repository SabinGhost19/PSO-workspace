#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void handle_USR2(int signal_nr)
{

    if (signal_nr == SIGUSR2)
    {

        printf("SIGUSR2 detecred\n");
        while (1)
        {
        }
    }
}
void handle_SIGINT(int sig_nr)
{
    if (sig_nr == SIGINT)
    {
        printf("SIG INT ..................!!!!!!!!!!!!!!!!\n");
    }
    printf("REVENIRE...:((((((\n\n");
}

int main()
{

    struct sigaction sa2;

    sa2.sa_handler = handle_SIGINT;
    sa2.sa_flags = SA_RESETHAND;
    sigaction(SIGINT, &sa2, NULL);

    sigset_t sig_set;
    sigemptyset(&sig_set);
    sigaddset(&sig_set, SIGUSR2);
    // INIT SIG_SET

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    printf("Proc: %ld", (long)getpid());
    fflush(stdout);
    while (1)
    {
        sa.sa_handler = handle_USR2;
        sa.sa_mask = sig_set;
        sigaction(SIGUSR2, &sa, NULL);

        printf("BLOCK\n");
        sleep(3);

        sigemptyset(&sig_set);
        sa.sa_mask = sig_set;

        sigaction(SIGUSR2, &sa, NULL);
        printf("UN_BLOCKED...!\n");
        sleep(3);
        // sigemptyset(&sig_set);
        // sa.sa_mask = sig_set;
        // sigaction(SIGUSR2, &sa, NULL);
        // // deblocam semnalul
        // printf("UN_BLOCK\n");
        // sleep(3);
    }
    return 0;
}
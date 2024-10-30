#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void handle_USR2(int signal_nr)
{
    if (signal_nr == SIGINT)
    {

        printf("SIGINT detecred\n");
    }
}

int main()
{

    sigset_t sig_set;
    sigemptyset(&sig_set);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    printf("Proc: %ld", (long)getpid());
    fflush(stdout);

    sa.sa_handler = handle_USR2;
    sa.sa_flags = SA_RESETHAND;

    sigaction(SIGINT, &sa, NULL);
    while (1)
    {
    }
    return 0;
}
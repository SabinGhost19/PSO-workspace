#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void sigInt_handle(int signal_number)
{
    printf("\nSIGINT (Ctrl+C) detected!!! Ignoring...");
}

void sigQuit_handle(int signal_number)
{
    printf("Semnalul sigQQQQQUIT!!!....\n");
}

int main()
{
    // definire masca de semnale
    sigset_t set;

    // definire structura pentru
    // a specifica ce actiuni sa fie
    // asociata cu un anumit semnal
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigInt_handle;

    // asocierea semnalului cu structura care contine handleul
    sigaction(SIGINT, &sa, NULL);

    sa.sa_handler = sigQuit_handle;
    // initializare masca de semnale cu 0
    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    while (1)
    {
        // blocare
        sigprocmask(SIG_BLOCK, &set, NULL);
        // blocheaza toate semnalele din set
        printf("Process started with PID: %d\n", getpid());

        printf("Sleep....5s\n");
        printf("DONE SLEEP..............\n");

        sigprocmask(SIG_UNBLOCK, &set, NULL);
        sleep(5);
    }
    return 0;
}
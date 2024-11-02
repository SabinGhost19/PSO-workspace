#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

// Handler pentru semnalul SIGUSR1
void handle_signal_USR1(int signal_nr, siginfo_t *info, void *context)
{
    printf("Semnal %d primit cu valoarea: %d\n", signal_nr, info->si_value.sival_int);
}

int main()
{
    pid_t pid = fork();

    if (pid == -1)
    { // Eroare la fork
        perror("Error la fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    { // Procesul copil
        // Configurăm handler-ul pentru SIGUSR1
        struct sigaction sa;
        sa.sa_flags = SA_SIGINFO;
        sa.sa_sigaction = handle_signal_USR1;
        sigemptyset(&sa.sa_mask);

        if (sigaction(SIGUSR1, &sa, NULL) == -1)
        {
            perror("Error la setarea handler-ului pentru SIGUSR1");
            exit(EXIT_FAILURE);
        }

        // Blocăm semnalul SIGUSR1 temporar
        sigset_t block_set;
        sigemptyset(&block_set);
        sigaddset(&block_set, SIGUSR1);
        sigprocmask(SIG_BLOCK, &block_set, NULL);

        printf("Semnalul este blocat în copil\n");
        sleep(2);

        printf("Semnalul este deblocat în copil. Așteptăm să fie procesat...\n");
        sigprocmask(SIG_UNBLOCK, &block_set, NULL);
        // sigset_t set;
        // sigfillset(&set); // Blocăm toate semnalele
        // sigdelset(&set, SIGUSR1);
        // sigsuspend(&set);

        printf("Procesul copil a terminat execuția.\n");
        exit(0);
    }
    else
    { // Procesul părinte
        // Setăm valoarea atașată semnalului
        union sigval value;
        value.sival_int = 45;

        sleep(1);
        // Trimitem semnalul SIGUSR1 către procesul copil folosind sigqueue
        if (sigqueue(pid, SIGUSR1, value) == -1)
        {
            perror("Eroare la trimiterea semnalului cu sigqueue");
            exit(EXIT_FAILURE);
        }
        printf("Semnal trimis din parinte\n");

        // Așteptăm finalizarea procesului copil
        wait(NULL);
        printf("Procesul părinte a terminat execuția.\n");
    }

    return 0;
}

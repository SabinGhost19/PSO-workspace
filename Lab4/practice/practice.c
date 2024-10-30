#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

void handle_sigint(int signal)
{
    printf("SIG INT CAPTURED!!\n");
    exit(0);
}

void setup_signal_handler(int signal, void (*handler)(int))
{
    struct sigaction sa;

    // Golim structura sigaction înainte de utilizare
    memset(&sa, 0, sizeof(sa));

    // Specificăm handler-ul care va trata semnalul
    sa.sa_handler = handler;

    // Setăm opțiunea de a bloca alte semnale în timpul manipulării acestuia
    // Sigaction blochează implicit același semnal până la finalizarea handler-ului
    sa.sa_flags = 0;

    // Configurăm handler-ul pentru semnalul specificat
    if (sigaction(signal, &sa, NULL) == -1)
    {
        perror("Eroare la configurarea handler-ului pentru semnal");
        exit(EXIT_FAILURE);
    }
}
int main()
{
    // cand un semnal ajunge la proces
    // kernelul de ocupa cu intreruperea temporara a executiei
    // pentu a apela functia de handler
    setup_signal_handler(SIGINT, handle_sigint);
    while (1)
    {
        printf("Procesul cu PID:%ld", (long)getpid());
        printf("Asteptare semnal...ctrl+c for SIGINT");
        sleep(10);
    }
    return 0;
}
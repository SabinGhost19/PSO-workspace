#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <sys/types.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <sys/signalfd.h>
#define SHM_NAME "shm_nameee"
#define SHM_SIZE 1024
#define NR_STUDENT 3

int main(void)
{
    fd_set rfds;
    struct timeval tv;
    int retval;

    sigset_t mask;
    int sfd;
    struct signalfd_siginfo fdsi;

    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    sfd = signalfd(-1, &mask, 0);

    printf("DSAAD %d\n", getpid());
    while (1)
    {
        FD_ZERO(&rfds);
        FD_SET(sfd, &rfds);

        tv.tv_sec = 5;
        tv.tv_usec = 0;

        retval = select(sfd + 1, &rfds, NULL, NULL, &tv);
        if (retval == -1)
            perror("select()");
        else if (retval)
        {
            printf("Data is available now.\n");
            int s = read(sfd, &fdsi, sizeof(struct signalfd_siginfo));
            if (fdsi.ssi_signo == SIGUSR1)
            {
                printf("Got SIGUSR1\n");
            }
            else
            {
                printf("NO NAME SIGGG for fd\n");
            }
        }
        else
            printf("TIMER OUT for 5 sec\n");
    }

    exit(EXIT_SUCCESS);
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    int nr_of_proc = 0;
    printf("Introduceti nr of proc:");
    scanf("%d", &nr_of_proc);

    for (int i = 0; i < nr_of_proc; i++)
    {
        pid_t child_pid = 0;
        switch (child_pid = fork())
        {
        case 0:
        {
            // child proc
            printf("Child with index: %d\n", i);
            exit(0);
        }
        case -1:
        {
            perror("Eroare la fork");
            exit(EXIT_FAILURE);
        }
        default:
        {
            // in parent....do nothing
        }
        }
    }

    // wait for all children
    pid_t child_pid;
    int status;
    while ((child_pid = wait(&status)) > 0)
    {
        if (WIFEXITED(status))
        {
            printf("Child with PID %d exited with status %d\n", child_pid, WEXITSTATUS(status));
        }
    }

    if (errno != ECHILD)
    {
        perror("wait");
        exit(EXIT_FAILURE);
    }

    wait(NULL);
    return 0;
}
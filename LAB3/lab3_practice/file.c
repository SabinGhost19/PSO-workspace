#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

typedef int pid_t;

void errExit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main()
{
    int status;
    pid_t Childpid;
    setvbuf(stdout, NULL, _IONBF, 0);
    // or //
    // setbuf(stdout, NULL);

    char *buff = "Child Process is executing...";
    switch (Childpid = fork())
    {
    case 0:
        ssize_t bytes_written =
            write(1, buff, strlen(buff));
        if (bytes_written < 0)
        {
            errExit("mkstmp error");
        }
        printf("Child PID: %ld", (long)getpid());
        sleep(1000);
        break;
    case -1:
        errExit("fork");
        break;
    default:
        waitpid(Childpid, &status, 0);
        printf("\nParent proccess PID=%ld\n\n", (long)getpid());

        if (WIFEXITED(status))
        {
            printf("Child exited with status %d\n", WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("Child terminated by signal %d\n", WTERMSIG(status));
        }

        break;
    }

    return 0;
}
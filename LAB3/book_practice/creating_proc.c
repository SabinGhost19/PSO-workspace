#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

typedef int pid_t;

static int idata = 111;

int main(int argc, char *argvp[])
{
    int istack = 222;
    pid_t childPid;

    switch (childPid = fork())
    {
    case -1:
        perror("Fork error");
        exit(-1);
    case 0:
        // code for the child
        istack *= 5;
        idata *= 5;
        break;
    default:
        // code for the parent
        sleep(3);
        // give time/chance for child proc to exec
        break;
    }

    printf("PID=%ld,%s idata=%d istack=%d\n\n", (long)getpid(),
           (childPid == 0 ? "child" : "parent"), idata, istack);

    return 0;
}
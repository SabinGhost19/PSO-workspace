#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define pathName "my_pipe"
int main()
{
    pid_t ret = mkfifo(pathName, 0644);
    if (ret < 0)
    {
        perror("eraore la crearea pipeului");
        exit(-1);
    }
    return 0;
}
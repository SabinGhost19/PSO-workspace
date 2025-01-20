#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
// #include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#define BUFF_SIZE (1 << 13)
#define TEXT "test_message"
#define NAME "/test_queue"

#define BUFF_SIZE (1 << 13)
char buf[BUFF_SIZE];

int main(int argc, char *argv[])
{
    unsigned int prio = 10;
    mqd_t m;
    int rc;

    m = mq_open(NAME, O_RDWR, 0666, NULL);
    while (1)
    {
        rc = mq_receive(m, buf, BUFF_SIZE, &prio);
        printf("received: %s\n", buf);
    }

    mq_close(m);
    mq_unlink(NAME);
    return 0;
}
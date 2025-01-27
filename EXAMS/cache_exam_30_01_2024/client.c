#define _GNU_SOURCE
#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <mqueue.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>
#define _MQ_1CCH__CLI "/cache__client"
#define _MQ_2CLI__CCH "/client__cache"
#define NAME "/test_queue"

#define TEXT "test message"
#define BUF_SIZE (1 << 13)
mqd_t m_2client_cache;
mqd_t m_1cache_client;

unsigned int prio = 10;
int main()
{
    char buffer[BUF_SIZE];
    char revc_buffer[BUF_SIZE];
    // fgets(buffer, BUF_SIZE, stdin);
    m_1cache_client = mq_open(_MQ_1CCH__CLI, O_RDWR, 0666, NULL);
    m_2client_cache = mq_open(_MQ_2CLI__CCH, O_RDWR, 0666, NULL);
    fgets(buffer, BUF_SIZE, stdin);
    int rc = mq_send(m_2client_cache, buffer, strlen(buffer), prio);
    rc = mq_receive(m_1cache_client, revc_buffer, BUF_SIZE, &prio);
    if (rc == -1)
    {
        perror("mq_receive failed");
        exit(1);
    }
    printf("Receiver din server: %s...........\n", revc_buffer);
    rc = mq_close(m_2client_cache);
    rc = mq_close(m_1cache_client);

    return 0;
}

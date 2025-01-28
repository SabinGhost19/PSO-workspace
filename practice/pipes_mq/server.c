#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>
#include <mqueue.h>
#define MQ_NAME "/my_message_queue"
mqd_t mq = 0;
int prio = 0;
char *generate_string()
{
    srand(getpid());
    char *new_string = (char *)malloc(sizeof(char) * 10);
    int i = 0;
    for (i = 0; i < 10; i++)
    {
        new_string[i] = rand() % 57 + 64;
    }
    new_string[i] = '\0';
    return new_string;
}
void child_routine()
{
    char *buffer = generate_string();
    printf("Send::%s\n", buffer);
    mq_send(mq, buffer, strlen(buffer), 0);
}
int main()
{

    mq = mq_open(MQ_NAME, O_CREAT | O_RDWR, 0666, NULL);
    for (int i = 0; i < 10; i++)
    {
        pid_t pid = fork();
        switch (pid)
        {
        case 0:
        {
            child_routine();
            exit(0);
        }
        case -1:
        {
            perror("error at fork");
            exit(EXIT_FAILURE);
        }
        default:
        {
        }
        }
    }
    char buffer[BUFSIZ];
    // for (int i = 0; i < 2; i++)
    int i = 0;
    while (1)
    {
        mq_receive(mq, buffer, sizeof(buffer), 0);
        printf("Received::%d::%s\n", i, buffer);
        i++;
    }
    return 0;
}
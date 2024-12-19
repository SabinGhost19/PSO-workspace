// ex1(exercitii laborator)
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE];
int producerIndex;
int consumerIndex;
int consumed_items = 0;

sem_t locuri_libere;
sem_t locuri_ocupate;

pthread_mutex_t producer_mutex;
pthread_mutex_t consumer_mutex;

int produced = 0;
int consumed = 0;

pthread_mutex_t producer_counter;
pthread_mutex_t consumer_counter;

int fd;
pthread_mutex_t fd_mutex;

void *producer_routine(void *params)
{
    pthread_mutex_lock(&producer_counter);
    while (produced < 100000)
    {
        produced++;
        pthread_mutex_unlock(&producer_counter);
        int numar = rand() % 991 + 10;

        sem_wait(&locuri_libere);

        pthread_mutex_lock(&producer_mutex);
        buffer[producerIndex] = numar;
        producerIndex = producerIndex + 1 % BUFFER_SIZE;
        pthread_mutex_unlock(&producer_mutex);

        sem_post(&locuri_ocupate);
        usleep(numar * 1000);
        pthread_mutex_lock(&producer_counter);
    }
    pthread_mutex_unlock(&producer_counter);
    return NULL;
}

void *consumer_routine(void *params)
{
    char buffer_1[16];
    pthread_mutex_lock(&consumer_counter);
    while (consumed <= 100000)
    {
        consumed++;
        pthread_mutex_unlock(&consumer_counter);

        sem_wait(&locuri_ocupate);

        pthread_mutex_lock(&consumer_mutex);
        int numar = buffer[consumerIndex];
        consumerIndex = consumerIndex + 1 % BUFFER_SIZE;
        pthread_mutex_unlock(&consumer_mutex);

        sprintf(buffer_1, "%d\n", numar);
        pthread_mutex_lock(&fd_mutex);
        write(fd, buffer_1, strlen(buffer_1));
        pthread_mutex_unlock(&fd_mutex);

        sem_post(&locuri_libere);
        usleep(100 * 1000);
        pthread_mutex_lock(&consumer_counter);
    }
    pthread_mutex_unlock(&consumer_counter);
    return NULL;
}

#define FILENAME_LOG_NAME "log.txt"
void init()
{
    fd = open(FILENAME_LOG_NAME, O_RDWR | O_CREAT | O_APPEND, 0640);
    sem_init(&locuri_libere, 0, BUFFER_SIZE);
    sem_init(&locuri_ocupate, 0, 0);
    srand(time(NULL));
    pthread_mutex_init(&producer_mutex, NULL);
    pthread_mutex_init(&consumer_counter, NULL);
    pthread_mutex_init(&fd_mutex, NULL);
    pthread_mutex_init(&consumer_counter, NULL);
    pthread_mutex_init(&producer_counter, NULL);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Not enough arhuments!\n");
        exit(-1);
    }

    init();

    int producerNumber = atoi(argv[1]);
    int consumerNumber = atoi(argv[2]);
    pthread_t *prod_id = malloc(sizeof(int) * producerNumber);
    pthread_t *cons_id = malloc(sizeof(int) * consumerNumber);
    for (int i = 0; i < producerNumber; i++)
    {
        pthread_create(prod_id + i, NULL, producer_routine, NULL);
    }
    for (int i = 0; i < consumerNumber; i++)
    {
        pthread_create(cons_id + i, NULL, consumer_routine, NULL);
    }

    for (int i = 0; i < producerNumber; i++)
    {
        pthread_join(prod_id[i], NULL);
    }
    for (int i = 0; i < consumerNumber; i++)
    {
        pthread_join(cons_id[i], NULL);
    }

    sleep(1000);
    close(fd);

    return 0;
}
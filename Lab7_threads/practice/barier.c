#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define MAX_THREADS 5
pthread_barrier_t barrier;
static int INTEGER = 0;

void *__routine(void *args)
{
    int integer = (int)args;
    printf("Integer si:...%d..thrad id actually\n", integer);

    int response_of_barrier = pthread_barrier_wait(&barrier);

    if (response_of_barrier == PTHREAD_BARRIER_SERIAL_THREAD)
    {
        // signal AUTOMATICALLy all other threads that are waiting...
        printf("LAST THREAD.....FLOOOODDD....\n");
    }

    printf("after the wait...i received the message\n");
    return NULL;
}

int main()
{
    pthread_barrier_init(&barrier, NULL, MAX_THREADS);
    pthread_t thread_array__[MAX_THREADS];

    for (int INTEGER = 0; INTEGER < 5; INTEGER++)
    {
        pthread_create(thread_array__ + INTEGER, NULL, __routine, (void *)INTEGER);
    }

    for (int INTEGER = 0; INTEGER < 5; INTEGER++)
    {
        pthread_join(thread_array__[INTEGER], NULL);
    }

    pthread_barrier_destroy(&barrier);
    return 0;
}
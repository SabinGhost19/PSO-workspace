#include <pthread.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_THREADS 5
#define CHAR_SIZE 10
pthread_key_t thread_key;
typedef int pid_t;

void clean_up_function(void *thread_key)
{
    printf("CLean-up function: %d\n", (int *)thread_key);
    free(thread_key);
    // {
    //     printf("Dezaloc successfull...\n");
    // }
    // else
    // {
    //     printf("Dezaloc FAILED...\n");
    //     exit(1);
    // }
    printf("...............................\n");
}
void *thread_routine(void *args)
{
    // store a pointer....
    pid_t thread_id = *(pid_t *)args;
    printf("Thread %d alloc...\n", thread_id);

    const char *new_alloc_char = (char *)malloc(sizeof(char) * CHAR_SIZE);
    strcpy(new_alloc_char, "OOIIOOIIOO");
    pthread_setspecific(thread_key, new_alloc_char);

    printf("Thread %d a setat specific data: %s\n", thread_id, new_alloc_char);

    return args;
}
int main(int argc, char *argv[])
{
    // create the threads

    pthread_key_create(&thread_key, clean_up_function);

    pthread_t vec_of_threads[MAX_THREADS];
    int values[MAX_THREADS] = {1, 2, 3, 4, 5};
    // or
    for (int i = 0; i < MAX_THREADS; i++)
    {
        values[i] = i;
    }

    for (int i = 0; i < MAX_THREADS; i++)
    {

        if (pthread_create(&vec_of_threads[i], NULL, thread_routine, (int *)&values[i]))
        {
            perror("Eroare la crearea thread-ului");
            exit(EXIT_FAILURE);
        }
    }

    void *return_val = NULL;
    for (int i = 0; i < MAX_THREADS; i++)
    {
        pthread_join(vec_of_threads[i], &return_val);
        printf("Valoarea threadului: %d este: %d\n", i, *(int *)return_val);
    }

    pthread_key_delete(thread_key);
    return 0;
}
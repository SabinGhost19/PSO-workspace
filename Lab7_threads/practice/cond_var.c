#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct ARG
{
    int integer;
    char string[10];
} ARG;

pthread_cond_t variabila_conditie;
pthread_mutex_t mutex;

void *routine_for_2(void *args)
{
    ARG *arg = (ARG *)args;

    printf("Argumentul threadului ce este executat:%d...%s\n", arg->integer, arg->string);
    printf("WAit pe cond_var\n");
    pthread_cond_wait(&variabila_conditie, &mutex);
    printf("am primit semnal si o sa imi termin executia dupa 2 sec\n");
    sleep(2);
    return NULL;
}

void *routine_for_1(void *args)
{
    ARG *arg = (ARG *)args;

    printf("Argumentul threadului ce este executat:%d...%s\n", arg->integer, arg->string);

    printf("Sleeping for 10 sec\n");
    sleep(10);
    printf("Ne gandim sa trimitem un semnal ca sa faca threadul 2 sa se trezeasaca\n");
    pthread_cond_signal(&variabila_conditie);
    printf("EU SUNT THREAD 1 SI IES....\n");
    return NULL;
}
pthread_t *create_new_thread(char *name, int nr)
{
    pthread_t *thread_id = (pthread_t *)malloc(sizeof(pthread_t));
    ARG *new_arg = (ARG *)malloc(sizeof(ARG));
    new_arg->integer = nr;
    strcpy(new_arg->string, name);
    pthread_create(thread_id, NULL, &routine_for_1, new_arg);
    return thread_id;
}
pthread_t *create_new_thread_2(char *name, int nr)
{
    pthread_t *thread_id = (pthread_t *)malloc(sizeof(pthread_t));
    ARG *new_arg = (ARG *)malloc(sizeof(ARG));
    new_arg->integer = nr;
    strcpy(new_arg->string, name);
    pthread_create(thread_id, NULL, &routine_for_2, new_arg);
    return thread_id;
}
int main()
{
    pthread_cond_init(&variabila_conditie, NULL);
    pthread_mutex_init(&mutex, NULL);

    pthread_t *td1 = create_new_thread("Thread 1", 1);
    pthread_t *td2 = create_new_thread_2("Thread 2", 2);

    pthread_join(*td1, NULL);
    pthread_join(*td2, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&variabila_conditie);
    return 0;
}
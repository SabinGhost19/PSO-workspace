#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define VEC_SIZE 100000

typedef struct our_args
{
    int integer;
    char string[10];
} our_args;

int global_vector[VEC_SIZE] = {0};
int value = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *
routine(void *arg)
{
    our_args *local_arg = (our_args *)arg;
    printf("Acesta este threadul 1111\n");
    printf("Integer:%d...String: %s\n", local_arg->integer, local_arg->string);

    for (int i = 0; i < VEC_SIZE; i++)
    {
        pthread_mutex_lock(&mutex);
        value += 10;
        pthread_mutex_unlock(&mutex);
    }
    local_arg->integer++;
    return &local_arg->integer;
}

void *routine_2(void *agrs)
{
    printf("Acesta este threadul 2222\n");

    for (int i = 0; i < VEC_SIZE; i++)
    {
        pthread_mutex_lock(&mutex);
        value += 10;
        pthread_mutex_unlock(&mutex);
    }
}
int main(int argc, char argv[])
{

    pthread_t first_thread;
    pthread_t second_thread;

    our_args args;
    args.integer = 10;
    memcpy(args.string, "STRING", strlen("STRING"));

    int id = pthread_create(&first_thread, NULL, routine, &args);
    int id2 = pthread_create(&second_thread, NULL, routine_2, &args);

    pthread_join(second_thread, NULL);
    void *returning = NULL;
    pthread_join(first_thread, &returning);
    printf("Valoera returnata: %d\n", *(int *)returning);

    int sum = 0;
    for (int i = 0; i < VEC_SIZE; i++)
    {
        sum += global_vector[i];
    }
    printf(".............SUMA ESTE: %d...............\n", value);
    return 0;
}

// //THREAD 1
// var_var+=1

// //THREAD 2
// var=var+1;

// var=10;

// mov eax,var                 mov eax,var
// ----SIGNAL                  var=var+10---20
// ---AND HANDLE               mov var,eax
// ---10
// var=var+10---20
// mov var,eax

// mov eax,var
// ----SIGNAL                  var=var+10---30
// ---AND HANDLE               mov var,eax

// ----var 30

// mov eax,adres_var
// add eax,1
// mov addres_var,eax
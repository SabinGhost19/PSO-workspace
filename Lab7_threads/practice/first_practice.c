#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

typedef struct _ThreadArgs
{
    int value;
    char string[10];
} _ThreadArgs;

void *second_routine(void *args)
{
    _ThreadArgs *_args1 = (_ThreadArgs *)args;

    sleep(1);
    printf("THIs is a code to run form a thread routine: %d\n", _args1->value);
    printf("String:...%s", _args1->string);
}
void *first_routine(void *args)
{
    _ThreadArgs *_args1 = (_ThreadArgs *)args;
    printf("THIs is a code to run form a thread routine: %d\n", _args1->value);
    _args1->value++;
    printf("String:...%s", _args1->string);
}
int main()
{
    pthread_t thread_id, thread_id_2;
    _ThreadArgs args1;
    args1.value = 1;
    strcpy(args1.string, "string");

    pthread_create(&thread_id, NULL, &first_routine, &args1);
    pthread_create(&thread_id_2, NULL, &second_routine, &args1);

    pthread_join(thread_id, NULL);
    pthread_join(thread_id_2, NULL);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutex;
long long xx = 0;
void *routine()
{

    // int *x = (int *)args;
    for (int i = 0; i < 1000000; i++)
    {
        pthread_mutex_lock(&mutex);
        xx += 1;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main()
{
    pthread_t thread_1;
    pthread_t thread_2;
    pthread_mutex_init(&mutex, NULL);
    if (pthread_create(&thread_1, NULL, &routine, NULL) != 0)
    {
        return 5;
    }
    if (pthread_create(&thread_2, NULL, &routine, NULL) != 0)
    {
        return 10;
    }

    if (pthread_join(thread_1, NULL) != 0)
    {
        return 7;
    }
    if (pthread_join(thread_2, NULL) != 0)
    {
        return 8;
    }
    printf("X final:%ld", xx);

    return 0;
}

// #include <stdlib.h>
// #include <stdio.h>
// #include <pthread.h>

// int mails = 0;
// pthread_mutex_t mutex;

// void *routine()
// {
//     for (int i = 0; i < 10000000; i++)
//     {
//         pthread_mutex_lock(&mutex);
//         mails++;
//         pthread_mutex_unlock(&mutex);
//         // read mails
//         // increment
//         // write mails
//     }
// }

// int main(int argc, char *argv[])
// {
//     pthread_t p1, p2, p3, p4;
//     pthread_mutex_init(&mutex, NULL);
//     if (pthread_create(&p1, NULL, &routine, NULL) != 0)
//     {
//         return 1;
//     }
//     if (pthread_create(&p2, NULL, &routine, NULL) != 0)
//     {
//         return 2;
//     }
//     if (pthread_create(&p3, NULL, &routine, NULL) != 0)
//     {
//         return 3;
//     }
//     if (pthread_create(&p4, NULL, &routine, NULL) != 0)
//     {
//         return 4;
//     }
//     if (pthread_join(p1, NULL) != 0)
//     {
//         return 5;
//     }
//     if (pthread_join(p2, NULL) != 0)
//     {
//         return 6;
//     }
//     if (pthread_join(p3, NULL) != 0)
//     {
//         return 7;
//     }
//     if (pthread_join(p4, NULL) != 0)
//     {
//         return 8;
//     }
//     pthread_mutex_destroy(&mutex);
//     printf("Number of mails: %d\n", mails);
//     return 0;
// }
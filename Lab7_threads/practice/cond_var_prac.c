#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#define BUFFER_MAX_SIZE 5

static int lenght = 0;
static int buffer[BUFFER_MAX_SIZE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_NOT_full_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_NOT_empty_cond = PTHREAD_COND_INITIALIZER;

typedef struct ARG
{
    int integer;
    char string[20];
} ARG;

void *routine__seller(void *args)
{
    ARG *arg = (ARG *)args;
    printf("%s...%d\n", arg->string, arg->integer);
    for (int i = 0; i < 5; i++)
    {
        pthread_mutex_lock(&mutex);
        while (lenght == 0)
        {
            pthread_cond_wait(&buffer_NOT_empty_cond, &mutex);
        }
        // daca s-a deblocat si storeul este maixm incepem sa scadem:
        int item = buffer[lenght--];
        printf("Sellerul a mai vandut un item\n");
        pthread_cond_signal(&buffer_NOT_full_cond);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return NULL;
}

void *routine__producer(void *args)
{

    ARG *arg = (ARG *)args;
    printf("%s...%d\n", arg->string, arg->integer);
    for (int i = 0; i < 5; i++)
    {
        pthread_mutex_lock(&mutex);

        while (lenght == BUFFER_MAX_SIZE)
        {
            pthread_cond_wait(&buffer_NOT_full_cond, &mutex);
        }

        buffer[lenght++] = 10;
        printf("Producerul a mai adaugat un item\n");
        pthread_cond_signal(&buffer_NOT_empty_cond);
        pthread_mutex_unlock(&mutex);
        sleep(2);
    }
    return NULL;
}
pthread_t fork_new_thread___producer__(int interger, char *string)
{
    ARG *arg1 = (ARG *)malloc(sizeof(ARG));
    arg1->integer = interger;
    strcpy(arg1->string, string);
    pthread_t t_id;
    pthread_create(&t_id, NULL, routine__producer, arg1);
    return t_id;
}
pthread_t fork_new_thread___seller__(int interger, char *string)
{
    ARG *arg1 = (ARG *)malloc(sizeof(ARG));
    arg1->integer = interger;
    strcpy(arg1->string, string);
    pthread_t t_id;
    pthread_create(&t_id, NULL, routine__seller, arg1);
    return t_id;
}

int main()
{
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&buffer_NOT_empty_cond, NULL);
    pthread_cond_init(&buffer_NOT_full_cond, NULL);

    pthread_t t1_id = fork_new_thread___producer__(1, "Producer...");
    pthread_t t2_id = fork_new_thread___seller__(2, "Seller...");

    pthread_join(t1_id, NULL);
    pthread_join(t2_id, NULL);

    return 0;
}

// // Buffer partajat
// #define BUFFER_SIZE 5
// int buffer[BUFFER_SIZE];
// int count = 0;

// // Mutex și variabile condiție
// pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t buffer_not_empty = PTHREAD_COND_INITIALIZER;
// pthread_cond_t buffer_not_full = PTHREAD_COND_INITIALIZER;

// // Funcția producătorului
// void *producer(void *arg)
// {
//     for (int i = 0; i < 10; i++)
//     {
//         int item = rand() % 100; // Producem un item aleatoriu

//         pthread_mutex_lock(&buffer_mutex); // Blocăm mutex-ul pentru acces exclusiv

//         // Așteptăm dacă buffer-ul este plin
//         while (count == BUFFER_SIZE)
//         {
//             pthread_cond_wait(&buffer_not_full, &buffer_mutex);
//         }

//         // Adăugăm item-ul în buffer
//         buffer[count++] = item;
//         printf("Producer: Produced item %d (count = %d)\n", item, count);

//         // Semnalizăm consumatorului că buffer-ul nu mai este gol
//         pthread_cond_signal(&buffer_not_empty);

//         pthread_mutex_unlock(&buffer_mutex); // Deblocăm mutex-ul
//         sleep(1);
//     }
//     return NULL;
// }

// // Funcția consumatorului
// void *consumer(void *arg)
// {
//     for (int i = 0; i < 10; i++)
//     {
//         pthread_mutex_lock(&buffer_mutex); // Blocăm mutex-ul pentru acces exclusiv

//         // Așteptăm dacă buffer-ul este gol
//         while (count == 0)
//         {
//             pthread_cond_wait(&buffer_not_empty, &buffer_mutex);
//         }

//         // Scoatem un item din buffer
//         int item = buffer[--count];
//         printf("Consumer: Consumed item %d (count = %d)\n", item, count);

//         // Semnalizăm producătorului că buffer-ul nu mai este plin
//         pthread_cond_signal(&buffer_not_full);

//         pthread_mutex_unlock(&buffer_mutex); // Deblocăm mutex-ul
//         sleep(2);
//     }
//     return NULL;
// }

// int main()
// {
//     pthread_t producer_thread, consumer_thread;

//     pthread_create(&producer_thread, NULL, producer, NULL);
//     pthread_create(&consumer_thread, NULL, consumer, NULL);

//     pthread_join(producer_thread, NULL);
//     pthread_join(consumer_thread, NULL);

//     return 0;
// }

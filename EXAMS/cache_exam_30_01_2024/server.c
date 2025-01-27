
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

#define NAME_SIZE 30
#define BUF_SIZE (1 << 13)
#define TEXT "serverrrrr"
#define _MQ_1CCH__CLI "/cache__client"
#define _MQ_2CLI__CCH "/client__cache"
#define CACHE_NAME "CACHE"
// #define CACHE_SIZE 10000
#define CACHE_SIZE (sizeof(CacheSlot) * 10)
#define MAX_THREADS 300
unsigned int PRIORITY = 10;
mqd_t m_2client_cache;
mqd_t m_1cache_client;
int current_index_in_list;
// Definirea structurii pentru cache
typedef struct CacheSlot
{
    void *entrance;
    char file_name[NAME_SIZE];
    int start_byte;
    char opp[NAME_SIZE];
    char to_write[BUF_SIZE];
    int nr_bytes;
    int _dirty_flag_;
    int __acces_rate;
    int recently_inserted;
} CacheSlot;
void *main_en;

void init()
{
    m_1cache_client = mq_open(_MQ_1CCH__CLI, O_CREAT | O_RDWR, 0666, NULL);
    m_2client_cache = mq_open(_MQ_2CLI__CCH, O_CREAT | O_RDWR, 0666, NULL);
}
void create_shared_pages_for_struct()
{

    int rc;
    int shm_fd;

    shm_fd = shm_open(CACHE_NAME, O_CREAT | O_RDWR, 0644);
    if (shm_fd == -1)
    {
        perror("shm_open failed");
        exit(1);
    }

    rc = ftruncate(shm_fd, CACHE_SIZE);
    if (rc == -1)
    {
        perror("ftruncate failed");
        exit(1);
    }

    main_en = mmap(0, CACHE_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
    if (main_en == MAP_FAILED)
    {
        perror("mmap failed");
        exit(1);
    }
    CacheSlot *entrance = (CacheSlot *)main_en;
    for (int i = 0; i < 10; i++)
    {
        entrance[i].__acces_rate = 0;
        entrance[i].recently_inserted = 0;
    }
}
int verify_hitting_type(CacheSlot *new_slot)
{
    printf("LRUUU\n");

    CacheSlot *entrance = (CacheSlot *)main_en;
    for (int i = 0; i < 10; i++)
    {
        if (strcmp(entrance[i].file_name, new_slot->file_name) == 0)
        {
            // if (entrance[i].start_byte <= new_slot->start_byte)
            //{
            //&&entrance[i].nr_bytes >= new_slot->nr_bytes;
            //  CAHCE HIT
            //  and server
            //  return NULL
            printf("LRUUU in verify\n");
            //}
            // mq_send(m_1cache_client, entrance[i].entrance, new_slot->nr_bytes, PRIORITY);
            return 0;
        }
    }
    return 1;
}
int LRU_Algo()
{
    printf("LRUUU\n");
    CacheSlot *entrance = (CacheSlot *)main_en;
    int min = BUF_SIZE;
    int index = 0;
    for (int i = 0; i < 10; i++)
    {
        if (entrance[i].__acces_rate < min)
        {
            min = entrance[i].__acces_rate;
            index = i;
        }
    }
    printf("index in LRU:%d\n", index);
    // mmap the mem
    // no need
    // get out
    entrance[index].__acces_rate = 0;
    munmap(entrance[index].entrance, entrance[index].nr_bytes * getpagesize());
    entrance[index].entrance = NULL;
    return index;
}
int next_valid_entrance()
{
    CacheSlot *entrance = (CacheSlot *)main_en;
    // if is no acces rate empty
    // empty entrance
    for (int i = 0; i < 10; i++)
    {
        if (entrance[i].__acces_rate == 0 && entrance[i].recently_inserted == 0)
        {
            return i;
        }
    }
    // make the lru and apply the algo
    int index = LRU_Algo();
    return index;
}
void *insert_new_Entrance(CacheSlot *new_slot)
{
    printf("Inserting in entrance\n");
    CacheSlot *entrance = (CacheSlot *)main_en;

    int first_index = next_valid_entrance();
    printf("Inserting in first index :%d\n", first_index);

    strcpy(entrance[first_index].file_name, new_slot->file_name);
    strcpy(entrance[first_index].opp, new_slot->opp);
    strcpy(entrance[first_index].to_write, new_slot->to_write);
    // 180-220

    entrance[first_index].nr_bytes = new_slot->nr_bytes;
    entrance[first_index].start_byte = new_slot->start_byte;
    entrance[first_index].__acces_rate++;

    int fd = open(new_slot->file_name, O_RDWR);
    // nr of new entrance
    // start byte =180
    // nr of bytes 40
    // 180+40/100=2
    int multiply = (entrance[first_index].start_byte + entrance[first_index].nr_bytes) / 100;
    int lower_margin = 0;
    if (entrance[first_index].start_byte != 0)
    {
        // 180/100
        // 1
        lower_margin = entrance[first_index].start_byte / 100;
    }

    entrance[first_index].entrance = mmap(NULL, 100, PROT_READ | PROT_WRITE, MAP_SHARED, fd, lower_margin * 100);
    entrance[first_index].recently_inserted = 1;
    // mapam 100-200
    int next_index = 0;
    for (int i = lower_margin + 1; i <= multiply; i++)
    {
        next_index = next_valid_entrance();
        // 200-300
        entrance[next_index].entrance = mmap(NULL, 100, PROT_READ | PROT_WRITE, MAP_SHARED, fd, i * 100);
        entrance[next_index].recently_inserted = 1;
    }
    // reset the recently_inserted index
    entrance[first_index].recently_inserted = 0;
    for (int i = lower_margin + 1; i <= multiply; i++)
    {
        entrance[i].recently_inserted = 0;
    }

    // 180-220
    char buffer[BUF_SIZE];
    strncpy(buffer, entrance[first_index].entrance + entrance[i].start_byte, entrance[i].nr_bytes);
    printf("Continutul fișierului mapat: %s\n", buffer);
    int rc = mq_send(m_1cache_client, buffer, strlen(buffer), PRIORITY);
    if (rc == -1)
    {
        perror("mq_send failed");
        close(fd);
        return NULL;
    }
    close(fd);
    return NULL;
    printf("ERROR AT INSERTING....WHATTT NIGGA??\n");
    exit(EXIT_FAILURE);
}
CacheSlot *make_new_slot(char *filename, char *opp, int start_bytes, int nr_bytes, char *to_write);
void *handle_connection(void *args)
{
    CacheSlot *new_slot = (CacheSlot *)args;
    printf("HANDLE IN THREAD: ....%s %s %s %d %d\n", new_slot->file_name, new_slot->opp, new_slot->to_write, new_slot->nr_bytes, new_slot->start_byte);

    if (verify_hitting_type(new_slot) == 0)
    {
        // CAHCE HIT
        // mq_send(m_1cache_client, entrance[i].entrance, new_slot->nr_bytes, PRIORITY);
    }
    else
    {
        // CACHE MISS
        // LRU_Algo();
        insert_new_Entrance(new_slot);
    }

    return NULL;
}
int main(int argc, char **argv)
{
    unsigned int prio = 10;
    int rc;
    init();
    create_shared_pages_for_struct();
    pid_t pid = fork();

    switch (pid)
    {
    case 0:
    {
        printf("Cache COPIL;.....\n");
        CacheSlot *cache_slots = (CacheSlot *)main_en;
        // for (int i = 0; i < 10; i++)
        // {
        //     printf("%s...%d...%d\n", cache_slots[i].file_name, cache_slots[i].nr_bytes, cache_slots[i].__acces_rate);
        // }
        // sleep(10);
        // printf("CHILDDD....%s\n", cache_slots[0].file_name);
        exit(0);
    }
    case -1:
    {
        perror("error creating fork");
        exit(EXIT_FAILURE);
    }
    default:
    {
        pthread_t tid[MAX_THREADS];
        int current_conn = 0;
        char buffer[BUF_SIZE];
        // parent
        while (1)
        {
            rc = mq_receive(m_2client_cache, buffer, BUF_SIZE, &prio);
            char filename[NAME_SIZE];
            int start_bytes = 0;
            char opp[NAME_SIZE];
            int nr_bytes = 0;
            char to_write[BUF_SIZE];
            sscanf(buffer, "%s %s %d %d %s", filename, opp, &start_bytes, &nr_bytes, to_write);
            // if (strcmp(opp, "write") == 0)
            // {
            //     // printf("REceived drom client WRITEEE: %s %s %d %d %s...\n", filename, opp, start_bytes, nr_bytes, to_write);
            // }
            // else
            // {
            //     // printf("llllllll: %s %s %d %d...\n", filename, opp, start_bytes, nr_bytes);
            // }
            CacheSlot *new_slot = make_new_slot(filename, opp, start_bytes, nr_bytes, to_write);
            pthread_create(&tid[current_conn], NULL, handle_connection, (void *)new_slot);
            // int rc = mq_send(m_1cache_client, TEXT, strlen(TEXT), prio);

            current_conn++;
        }

        rc = mq_close(m_2client_cache);
        rc = mq_close(m_1cache_client);
        rc = munmap(main_en, CACHE_SIZE);
        rc = shm_unlink(CACHE_NAME);

        return 0;
    }
    }
    return 0;
}
CacheSlot *make_new_slot(char *filename, char *opp, int start_bytes, int nr_bytes, char *to_write)
{
    CacheSlot *new_cache = (CacheSlot *)malloc(sizeof(CacheSlot));
    strcpy(new_cache->file_name, filename);
    strcpy(new_cache->opp, opp);
    new_cache->start_byte = start_bytes;
    new_cache->nr_bytes = nr_bytes;
    if (strcmp(opp, "write") == 0)
    {
        strcpy(new_cache->to_write, to_write);
    }
    else
    {
        strcpy(new_cache->to_write, "0");
    }
    return new_cache;
}

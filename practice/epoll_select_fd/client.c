#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#define SHm_NAME "/shm"
#define SEM_NAME "/elev_sem"
#define STUDENT_COUNTER 3
#define SEM_STRUCT_LONG sizeof(sem_t *)
typedef struct student
{
    char nume[20];
    int nr_note;
    int note[10];
} student;
int current_note = 0;
void *global_entry = NULL;
void open_sh_map_and_map_that()
{

    int shm_fd = shm_open(SHm_NAME, O_RDWR, 0666);

    if (ftruncate(shm_fd, sizeof(student) * STUDENT_COUNTER) == -1)
        exit(EXIT_FAILURE);

    global_entry = mmap(NULL, sizeof(student) * STUDENT_COUNTER, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (global_entry == MAP_FAILED)
        exit(EXIT_FAILURE);
}
void make_media(int index)
{
    float sum = 0;
    student *students_ptr = ((student *)(global_entry + SEM_STRUCT_LONG));

    for (int i = 0; i < current_note; i++)
    {
        sum += students_ptr[index].note[i];
    }
    sum /= current_note;
    printf("Media este: %lf...pentur %s\n", sum, students_ptr[index].nume);
}
void make_the_media()
{
    sem_t *sem = (sem_t *)global_entry;

    for (int j = 0; j < 20; j++)
    {
        printf("Waiting to be written in the mem.......\n");
        sem_wait(sem);
        for (int i = 0; i < STUDENT_COUNTER; i++)
        {
            make_media(i);
        }
        current_note++;
    }
}
void handle_USR1(int sig)
{
    if (sig == SIGUSR1)
    {
        printf("SigUSR1 received\n");
        open_sh_map_and_map_that();
        make_the_media();
    }
}
void signal_handler()
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_USR1;
    sigaction(SIGUSR1, &sa, NULL);
}
int main()
{
    // shm_unlink(SHm_NAME);
    printf("PID:%d\n", getpid());
    // not create
    signal_handler();
    // open_sh_map_and_map_that();
    // make_the_media();

    sleep(200);
    return 0;
}
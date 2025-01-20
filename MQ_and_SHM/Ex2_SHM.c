#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>    /* For O_* constants */
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define SEM_NAME "my_semaphore"
#define SHM_NAME "SH_MEM_NAME"
#define SHM_SIZE 1024

struct student
{
    int note[10];
    char nume[20];
    int nr_note;
};

int main(void)
{
    void *mem;  /* map address */
    int shm_fd; /* memory descriptor */
    int rc, pvalue;
    sem_t *my_sem;
    int nr_to_send = 0;
    /* create shm */
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0644);
    rc = ftruncate(shm_fd, SHM_SIZE);

    my_sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);

    mem = mmap(0, 4096, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);

    struct student *student_s = mem;
    strcpy(student_s[0].nume, "Sabin");
    strcpy(student_s[1].nume, "Flore");

    while (nr_to_send <= 20)
    {
        sem_wait(my_sem);
        sleep(3);
        student_s[0].note[student_s[0].nr_note++] = rand() % 10 + 1;
        student_s[1].note[student_s[1].nr_note++] = rand() % 10 + 1;
        // send
        nr_to_send++;

        sem_post(my_sem);
    }
    close((int)mem);
    rc = sem_close(my_sem);
    return 0;
}
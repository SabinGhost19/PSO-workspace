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
#include <signal.h>
#include <time.h>

#define SHM_NAME "/mem_ttt"           // Numele segmentului de memorie partajată
#define SEM_NAME "/elev_sem"          // Numele semaforului partajat
#define STUDENT_COUNTER 3             // Numarul de studenti
#define SEM_STRUCT_LONG sizeof(sem_t) // Dimensiunea semaforului

typedef struct student
{
    char nume[20];
    int nr_note;
    int note[10];
} student;

int current_note = 0;
void *global_entry = NULL; // Punte la memoria partajată
sem_t *sem;                // Semaforul care va fi utilizat

// Crearea memoriei partajate și maparea acesteia în spațiul de memorie al procesului
void create_sh_map_and_map_that()
{
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666); // Crează fișierul de memorie partajată
    if (ftruncate(shm_fd, sizeof(student) * STUDENT_COUNTER) == -1)
    {
        perror("Error with ftruncate");
        exit(EXIT_FAILURE);
    }

    global_entry = mmap(NULL, sizeof(student) * STUDENT_COUNTER, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (global_entry == MAP_FAILED)
    {
        perror("Error with mmap");
        exit(EXIT_FAILURE);
    }
}
void sem_the_client()
{
    printf("Intordu pid: \n");
    pid_t pid = 0;
    scanf("%d", &pid);
    kill(pid, SIGUSR1);
    printf("Client semnalize...\n");
}
// Inițializarea semaforului în memoria partajată
void insert_the_sem()
{
    // Creăm semaforul în memoria partajată
    sem = (sem_t *)global_entry; // Semaforul este stocat la începutul memoriei partajate
    if (sem_init(sem, 1, 0) != 0)
    { // Inițializăm semaforul (2: 1 pentru procese multiple, valoare 1)
        perror("sem_init failed");
        exit(EXIT_FAILURE);
    }
}

int random_note_gen()
{
    return (rand() % 10) + 1;
}

int main()
{
    srand(time(NULL));
    sem_unlink(SEM_NAME);

    create_sh_map_and_map_that();
    insert_the_sem();
    sem_the_client();

    student *students_ptr = (student *)(global_entry + SEM_STRUCT_LONG);

    strcpy(students_ptr[0].nume, "sABIN");
    strcpy(students_ptr[1].nume, "george");
    strcpy(students_ptr[2].nume, "Mara");

    for (int i = 0; i < STUDENT_COUNTER; i++)
    {
        sleep(5);
        for (int j = 0; j < STUDENT_COUNTER; j++)
        {
            int nota = random_note_gen();
            students_ptr[j].note[current_note] = nota;
            // printf("Elev name: %s...\n", students_ptr[j].nume);
            // printf("Elev nota: %d...\n", students_ptr[j].note[current_note]);
        }
        sem_post(sem);
    }
    sem_close(sem);
    sem_unlink(SEM_NAME);
    return 0;
}

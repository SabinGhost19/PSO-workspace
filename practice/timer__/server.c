// #include <stdio.h>
// #include <stdlib.h>
// #include <pthread.h>
// #include <unistd.h>
// #include <signal.h>
// #include <string.h>
// #include <time.h>

// #define MAX_TASKS 5

// // Structura pentru taskuri
// typedef struct Task
// {
//     int task_id;
//     char filename[50];
// } Task;

// // Lista de taskuri
// Task task_list[MAX_TASKS];
// int task_count = 0;                                     // Numărul de taskuri în listă
// pthread_mutex_t task_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex pentru protejarea accesului la lista de taskuri
// pthread_cond_t task_cond = PTHREAD_COND_INITIALIZER;    // Condiție pentru a aștepta taskuri

// // Timer pentru SIGUSR2
// timer_t timer_id;

// // Flag pentru a opri procesul
// volatile sig_atomic_t stop_threads = 0;

// // Funcția care va fi apelată de timerul SIGUSR2
// void timer_handler(union sigval sv)
// {
//     printf("No tasks in the list for 10 seconds, sending SIGUSR2\n");
//     kill(getpid(), SIGUSR2);
// }

// // Funcția care procesează taskurile
// void *process_task(void *args)
// {
//     while (!stop_threads)
//     {
//         Task task;

//         pthread_mutex_lock(&task_mutex);

//         // Verifică flag-ul înainte de așteptare
//         if (stop_threads)
//         {
//             pthread_mutex_unlock(&task_mutex);
//             return NULL;
//         }

//         while (task_count == 0)
//         {
//             pthread_cond_wait(&task_cond, &task_mutex); // Așteaptă până când există un task
//         }
//         if (stop_threads)
//         {
//             pthread_mutex_unlock(&task_mutex);
//             return NULL;
//         }
//         // Preia un task din listă
//         task = task_list[0];
//         for (int i = 0; i < task_count - 1; i++)
//         {
//             task_list[i] = task_list[i + 1];
//         }
//         task_count--; // Scade numărul de taskuri

//         pthread_mutex_unlock(&task_mutex);

//         // Procesează taskul
//         printf("Processing task: %s\n", task.filename);
//         // sleep(1); // Simulează procesarea

//         // Dacă lista devine goală, setează timerul
//         if (task_count == 0)
//         {
//             struct itimerspec ts;
//             ts.it_value.tv_sec = 2; // Setează timerul pentru 10 secunde
//             ts.it_value.tv_nsec = 0;
//             ts.it_interval.tv_sec = 0; // Nu repetăm timerul
//             ts.it_interval.tv_nsec = 0;
//             timer_settime(timer_id, 0, &ts, NULL);
//         }
//     }
//     return NULL;
// }

// // Funcția care adaugă un task în listă
// void add_task(Task task)
// {
//     pthread_mutex_lock(&task_mutex);
//     if (task_count < MAX_TASKS)
//     {
//         task_list[task_count] = task;
//         task_count++;
//         pthread_mutex_unlock(&task_mutex);

//         // Semnalizează că există un task disponibil
//         pthread_cond_signal(&task_cond);

//         // Dacă lista nu mai este goală, anulăm timerul
//         if (task_count > 0)
//         {
//             struct itimerspec ts = {{0, 0}, {0, 0}}; // Setează timerul pentru a fi anulat
//             timer_settime(timer_id, 0, &ts, NULL);
//         }
//     }
//     else
//     {
//         printf("Task list is full\n");
//         pthread_mutex_unlock(&task_mutex);
//     }
// }

// // Funcția pentru gestionarea semnalului SIGUSR2
// void signal_handler(int sig)
// {
//     printf("Received SIGUSR2 signal. Exiting...\n");
//     stop_threads = 1;                   // Setează flag-ul pentru oprirea threadurilor
//     pthread_cond_broadcast(&task_cond); // Trezește toate threadurile care așteaptă
// }

// // Funcția de inițializare
// void init(int argc)
// {
//     struct sigaction sa;
//     sa.sa_handler = signal_handler;
//     sa.sa_flags = 0;
//     sigemptyset(&sa.sa_mask);

//     if (sigaction(SIGUSR2, &sa, NULL) == -1)
//     {
//         perror("sigaction");
//         exit(EXIT_FAILURE);
//     }
// }

// int main(int argc, char *argv[])
// {
//     init(argc);

//     // Crearea timerului
//     struct sigevent sev;
//     sev.sigev_notify = SIGEV_THREAD;
//     sev.sigev_notify_function = timer_handler;
//     sev.sigev_notify_attributes = NULL;
//     sev.sigev_value.sival_ptr = NULL;
//     timer_create(CLOCK_REALTIME, &sev, &timer_id);

//     // Crearea threadurilor
//     pthread_t threads[3];
//     for (int i = 0; i < 3; i++)
//     {
//         if (pthread_create(&threads[i], NULL, process_task, NULL) != 0)
//         {
//             perror("Thread creation failed");
//             exit(EXIT_FAILURE);
//         }
//     }

//     // Adăugarea de taskuri
//     for (int i = 0; i < 10; i++)
//     {
//         Task new_task = {i, "task_filename"};
//         add_task(new_task);
//     }

//     // Așteaptă terminarea threadurilor
//     for (int i = 0; i < 3; i++)
//     {
//         pthread_join(threads[i], NULL); // Așteaptă fiecare thread să se termine
//     }

//     // Distruge timerul
//     timer_delete(timer_id);

//     return 0;
// }

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#define MAX_TASKS 5

// Structura pentru taskuri
typedef struct Task
{
    int task_id;
    char filename[50];
} Task;

// Lista de taskuri
Task task_list[MAX_TASKS];
int task_count = 0;                                     // Numărul de taskuri în listă
pthread_mutex_t task_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex pentru protejarea accesului la lista de taskuri
pthread_cond_t task_cond = PTHREAD_COND_INITIALIZER;    // Condiție pentru a aștepta taskuri

// Timer pentru SIGUSR2
timer_t timer_id;

// Flag pentru a opri procesul
volatile sig_atomic_t stop_threads = 0;

// Funcția care va fi apelată de timerul SIGUSR2
void timer_handler(union sigval sv)
{
    printf("No tasks in the list for 10 seconds, sending SIGUSR2\n");
    kill(getpid(), SIGUSR2);
}

// Funcția care procesează taskurile
void *process_task(void *args)
{
    while (!stop_threads)
    {
        Task task;

        pthread_mutex_lock(&task_mutex);

        // Verifică flag-ul înainte de așteptare
        if (stop_threads)
        {
            pthread_mutex_unlock(&task_mutex);
            return NULL;
        }

        while (task_count == 0)
        {
            pthread_cond_wait(&task_cond, &task_mutex); // Așteaptă până când există un task
        }

        // Preia un task din listă
        task = task_list[0];
        for (int i = 0; i < task_count - 1; i++)
        {
            task_list[i] = task_list[i + 1];
        }
        task_count--; // Scade numărul de taskuri

        pthread_mutex_unlock(&task_mutex);

        // Procesează taskul
        printf("Processing task: %s\n", task.filename);
        sleep(1); // Simulează procesarea

        // Dacă lista devine goală, setează timerul
        if (task_count == 0)
        {
            struct itimerspec ts;
            ts.it_value.tv_sec = 2; // Setează timerul pentru 10 secunde
            ts.it_value.tv_nsec = 0;
            ts.it_interval.tv_sec = 0; // Nu repetăm timerul
            ts.it_interval.tv_nsec = 0;
            timer_settime(timer_id, 0, &ts, NULL);
        }
    }
    return NULL;
}

// Funcția care adaugă un task în listă
void add_task(Task task)
{
    pthread_mutex_lock(&task_mutex);
    if (task_count < MAX_TASKS)
    {
        task_list[task_count] = task;
        task_count++;
        pthread_mutex_unlock(&task_mutex);

        // Semnalizează că există un task disponibil
        pthread_cond_signal(&task_cond);

        // Dacă lista nu mai este goală, anulăm timerul
        if (task_count > 0)
        {
            struct itimerspec ts = {{0, 0}, {0, 0}}; // Setează timerul pentru a fi anulat
            timer_settime(timer_id, 0, &ts, NULL);
        }
    }
    else
    {
        printf("Task list is full\n");
        pthread_mutex_unlock(&task_mutex);
    }
}

// Funcția pentru gestionarea semnalului SIGUSR2
void signal_handler(int sig)
{
    printf("Received SIGUSR2 signal. Exiting...\n");
    stop_threads = 1;                   // Setează flag-ul pentru oprirea threadurilor
    pthread_cond_broadcast(&task_cond); // Trezește toate threadurile care așteaptă
}

// Funcția de inițializare
void init(int argc)
{
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR2, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    init(argc);

    // Crearea timerului
    struct sigevent sev;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = timer_handler;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = NULL;
    timer_create(CLOCK_REALTIME, &sev, &timer_id);

    // Crearea threadurilor
    pthread_t threads[3];
    for (int i = 0; i < 3; i++)
    {
        if (pthread_create(&threads[i], NULL, process_task, NULL) != 0)
        {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }

    // Adăugarea de taskuri
    for (int i = 0; i < 10; i++)
    {
        Task new_task = {i, "task_filename"};
        add_task(new_task);
    }

    // Așteaptă terminarea threadurilor
    for (int i = 0; i < 3; i++)
    {
        pthread_join(threads[i], NULL); // Așteaptă fiecare thread să se termine
    }

    // Distruge timerul
    timer_delete(timer_id);

    return 0;
}

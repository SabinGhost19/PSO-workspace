#include "planner.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct planner
{
    // numarul de dispozoitive
    unsigned int nr_of_event;
    // cuanta de timp dupa care un procc trebuie preemptat
    unsigned int time_quantum;
    // current thread
    tid_t current_thread_in_execution;

    // priority queue of all priority lvls
    _thread_task *ready_queues[SO_MAX_PRIO];
} _scheduler;

typedef struct thread_task
{
    pthread_t thread_id;
    unsigned int priority;
    so_handler *handler;
} _thread_task;

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
void enqueue(_thread_task **queue, _thread_task *task)
{
    if (*queue == NULL)
    {
        *queue = task;
    }
    else
    {
        _thread_task *temp = *queue;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = task;
    }
    task->next = NULL;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
_thread_task *dequeue(thread_task **queue)
{
    if (*queue == NULL)
    {
        return NULL;
    }
    _thread_task *task = *queue;
    *queue = (*queue)->next;
    task->next = NULL;
    return task;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
_thread_task *select_next_task()
{
    for (int i = SO_MAX_PRIO - 1; i >= 0; i--)
    {
        if (scheduler.ready_queues[i] != NULL)
        {
            return dequeue(&scheduler.ready_queues[i]);
        }
    }
    return NULL;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
void functie_rutina_thread__2_(unsigned int priority)
{
    printf("Aceasta este o rutina ce trebuie executata...priority: %d\n", priority);
    printf("Executare functie din thread 2 \n");
}
void functie_rutina_thread__1_(unsigned int priority)
{
    printf("Aceasta este o rutina ce trebuie executata...priority: %d\n", priority);
    tid_t tid = so_fork(functie_rutina_thread__2_, 2);
    sleep(10);
    printf("Forking new thread....thread 2 is generated and executing..\n");
    // waiting for thread 2 to finish its exevcution;
    pthread_join(tid, NULL);
}

void *execute_the_thread_with_task(void *args)
{
    _thread_task *task = (_thread_task *)args;
    task->handler(task->priority);

    // GET THE NEXT EXISTING TASK IN THE QUEUEU....
    // and execute it
    _thread_task current_thread = select_next_task();
    current_thread.handler();

    return NULL;
}

tid_t so_fork(so_handler *func, unsigned int priority)
{
    _thread_task *new_thread_task = (_thread_task *)malloc(sizeof(_thread_task));
    new_thread_task->handler = func;
    new_thread_task->priority = priority;
    if (pthread_create(&new_thread_task->thread_id, NULL, execute_the_thread_with_task, new_thread_task) != 0)
    {
        perror("Eroare la pthread_create");
        return INVALID_TID;
    }

    // ENQUEUE THE NEW TASK in the QUEUEU....
    //------------------------------------------------------------
    enqueue(scheduler.ready_queues, new_thread_task);
    return new_thread_task->thread_id;
}

_scheduler scheduler;
int so_init(unsigned int time_quantum, unsigned int io)
{
    if (io > SO_MAX_NUM_EVENTS)
    {
        fprintf(stderr, "Numărul de evenimente depășește limita permisă.\n");
        return -1;
    }

    scheduler.nr_of_event = io;
    scheduler.time_quantum = time_quantum;
}
int main()
{

    // rethink the logic of the algorithm!!!!....
    so_init(10, 5);
    tid_t tid = so_fork(functie_rutina_thread__1_, 0);

    pthread_join(tid, NULL);
    return 0;
}
#include "planner.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct _thread_task
{
    unsigned int time_slice;
    char state[10];
    pthread_t thread_id;
    unsigned int priority;
    so_handler *handler;
    struct _thread_task *next;
} _thread_task;

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

    pthread_cond_t conditional_val_NEXT_Thread_FIND__;
    pthread_cond_t conditional_val_Thread_Start;
    pthread_mutex_t ___mutex_;
}

_scheduler;

static _scheduler scheduler;
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
_thread_task *dequeue(_thread_task **queue)
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
    sleep(2);
}
void functie_rutina_thread__1_(unsigned int priority)
{
    printf("Aceasta este o rutina ce trebuie executata...priority: %d\n", priority);
    printf("Forking new thread....thread 2 is generated and executing..\n");
    tid_t tid = so_fork(functie_rutina_thread__2_, 3, 20);
    // waiting for thread 2 to finish its exevcution;
    printf("CONTINUAM DE LA PRIMA FUNCTIE.............\n");
    pthread_join(tid, NULL);
}

void *execute_the_thread_with_task(void *args)
{
    _thread_task *task = (_thread_task *)args;

    while (task->time_slice > 0)
    {
        pthread_mutex_lock(&scheduler.___mutex_);

        while (scheduler.current_thread_in_execution != task->thread_id)
        {
            pthread_cond_wait(&scheduler.conditional_val_Thread_Start, &scheduler.___mutex_);
        }
        strcpy(task->state, "RUNNING");

        task->handler(task->priority);
        // exec the task.....
        task->time_slice -= scheduler.time_quantum;
        // if the standard time quant exited normally....substract the time
        if (task->time_slice > 0)
        {
            enqueue(&scheduler.ready_queues[task->priority], task);
        }

        strcpy(task->state, "WAITING");
        // signal for next thread to run
        pthread_cond_signal(&scheduler.conditional_val_NEXT_Thread_FIND__);

        pthread_mutex_unlock(&scheduler.___mutex_);
    }
    return NULL;
}

tid_t so_fork(so_handler *func, unsigned int priority, unsigned int time_slice)
{
    pthread_mutex_unlock(&scheduler.___mutex_);
    _thread_task *new_thread_task = (_thread_task *)malloc(sizeof(_thread_task));
    new_thread_task->handler = func;
    new_thread_task->priority = priority;
    new_thread_task->time_slice = time_slice;
    strcpy(new_thread_task->state, "READY");
    new_thread_task->next = NULL;

    if (pthread_create(&new_thread_task->thread_id, NULL, execute_the_thread_with_task, new_thread_task) != 0)
    {
        perror("Eroare la pthread_create");
        return INVALID_TID;
    }

    // ENQUEUE THE NEW TASK in the QUEUEU....
    //------------------------------------------------------------
    pthread_mutex_lock(&scheduler.___mutex_);
    enqueue(&scheduler.ready_queues[new_thread_task->priority], new_thread_task);
    pthread_cond_signal(&scheduler.conditional_val_NEXT_Thread_FIND__);
    pthread_mutex_unlock(&scheduler.___mutex_);

    return new_thread_task->thread_id;
}

int so_init(unsigned int time_quantum, unsigned int io)
{
    if (io > SO_MAX_NUM_EVENTS)
    {
        fprintf(stderr, "Numărul de evenimente depășește limita permisă.\n");
        return -1;
    }
    //  pthread_cond_t condi pthread_cond_signal(&scheduler.conditional_val_NEXT_Thread_FIND__);tional_val_NEXT_Thread_FIND__ = PTHREAD_COND_INITIALIZER;
    // pthread_cond_t conditional_val_Thread_Start = PTHREAD_COND_INITIALIZER;
    // pthread_mutex_t ___mutex_ = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init(&scheduler.___mutex_, NULL);
    pthread_cond_init(&scheduler.conditional_val_NEXT_Thread_FIND__, NULL);
    pthread_cond_init(&scheduler.conditional_val_Thread_Start, NULL);
    scheduler.nr_of_event = io;
    scheduler.time_quantum = time_quantum;
    scheduler.current_thread_in_execution = INVALID_TID;

    for (int i = 0; i < SO_MAX_PRIO; i++)
    {
        scheduler.ready_queues[i] = NULL;
    }
    return 0;
}

void so_end()
{
    pthread_cond_destroy(&scheduler.conditional_val_NEXT_Thread_FIND__);
    pthread_cond_destroy(&scheduler.conditional_val_Thread_Start);
    pthread_mutex_destroy(&scheduler.___mutex_);
}
void so_runnnn()
{
    pthread_mutex_lock(&scheduler.___mutex_);
    // the queu is empty...no more threads come to be processed
    while (1)
    {

        _thread_task *current_thread = select_next_task();
        if (current_thread == NULL)
        {
            // end the run,queueu is emopty
            break;
        }
        scheduler.current_thread_in_execution = current_thread->thread_id;

        pthread_cond_signal(&scheduler.conditional_val_Thread_Start);

        pthread_cond_wait(&scheduler.conditional_val_NEXT_Thread_FIND__, &scheduler.___mutex_);
    }
    pthread_mutex_unlock(&scheduler.___mutex_);
}

int main()
{

    // rethink the logic of the algorithm!!!!....
    so_init(10, 5);
    tid_t tid = so_fork(functie_rutina_thread__1_, 0, 20);
    // tid_t tid2 = so_fork(functie_rutina_thread__2_, 2, 50);

    so_runnnn();
    pthread_join(tid, NULL);
    // pthread_join(tid2, NULL);

    so_end();

    return 0;
}

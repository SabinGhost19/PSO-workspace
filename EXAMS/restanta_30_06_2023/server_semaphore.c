#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>

#define MAX_THREADS 3
#define MAX_TASKS 5
#define FILENAME_DIM 30
typedef struct Task
{
    int sum;
    char filename[FILENAME_DIM]
} Task;
Task task_vector[MAX_TASKS];
long long global_sum = 0;
int vector_COUNT__ = 0;
void init(int argc);
void read_files(int argc, char *argv[]);
void submit_Task(Task new_task);
volatile sig_atomic_t stop_threads = 0;
pthread_mutex_t _task_mutex_ = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t __sum_mutex_ = PTHREAD_MUTEX_INITIALIZER;

sem_t file_to_write_available;
sem_t task_to_execute_available;

void execute_the_task(Task task_to_execute)
{
    printf("EXECUTING THE TASk: %s\n", task_to_execute.filename);
    // sleep(1);
    int fd = open(task_to_execute.filename, O_RDONLY);
    if (fd == -1)
    {
        perror("error open file");
        exit(EXIT_FAILURE);
    }
    char buffer[1024];
    int read_bytes = read(fd, buffer, 1024 * sizeof(char));
    printf("%s__>>> %s\n", task_to_execute.filename, buffer);

    pthread_mutex_lock(&__sum_mutex_);
    char *start = strtok(buffer, "\n");
    while (start != NULL)
    {
        int number = atoi(start);

        task_to_execute.sum += number;
        printf(".....Number %s: %d.......\n", task_to_execute.filename, number);
        start = strtok(NULL, "\n");
    }
    pthread_mutex_unlock(&__sum_mutex_);

    printf("Read bytes:%s from %s\n", buffer, task_to_execute.filename);
    printf("The sum is : %d......\n", task_to_execute.sum);

    pthread_mutex_lock(&__sum_mutex_);
    global_sum += task_to_execute.sum;
    printf("GLOBAL SUM : %lld......\n", global_sum);
    pthread_mutex_unlock(&__sum_mutex_);
    printf("PID Is %d\n", getpid());
}
int check_if_vector_is_empty()
{
    if (vector_COUNT__ == 0)
    {
        return 0;
    }
    return 1;
}
void *starting_routine(void *args)
{
    while (!stop_threads)
    {

        Task taken_task;
        if (stop_threads)
        {
            break;
        }
        sem_wait(&task_to_execute_available);

        pthread_mutex_lock(&_task_mutex_);

        taken_task = task_vector[0];

        for (int i = 0; i < vector_COUNT__; i++)
        {
            task_vector[i] = task_vector[i + 1];
        }
        vector_COUNT__--;
        // if (check_if_vector_is_empty() == 0)
        // {
        //     // timer
        //     // TODO:TIMER
        // }
        pthread_mutex_unlock(&_task_mutex_);
        sem_post(&file_to_write_available);
        execute_the_task(taken_task);
    }
    return NULL;
}

void *process_task(void *args)
{
    char *buffer = (char *)args;
    char *start = strtok(buffer, "\n");

    while (start != NULL)
    {
        int number;

        // Protejează accesul concurent la variabila `number` cu un mutex
        // pthread_mutex_lock(&_task_mutex_);

        number = atoi(start); // Apelăm atoi într-un mod thread-safe

        printf("Number: %d\n", number);

        // pthread_mutex_unlock(&_task_mutex_);
        //  Continuă cu următorul token
        start = strtok(NULL, "\n");
    }

    return NULL;
}

int main()
{
    pthread_t thread1, thread2, thread3, thread4, thread5, thread6;
    char data[] = "10234567\n20\n30\n40\n50\n";

    // Crearea threadurilor care vor procesa aceeași listă de taskuri
    pthread_create(&thread1, NULL, process_task, (void *)data);
    pthread_create(&thread2, NULL, process_task, (void *)data);
    pthread_create(&thread3, NULL, process_task, (void *)data);
    pthread_create(&thread4, NULL, process_task, (void *)data);
    pthread_create(&thread5, NULL, process_task, (void *)data);
    pthread_create(&thread6, NULL, process_task, (void *)data);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);
    pthread_join(thread5, NULL);
    pthread_join(thread6, NULL);

    return 0;
}
// int main(int argc, char *argv[])
// {

//     // int n = 0, sum = 0;
//     // printf("Insert the number/above limit:\n");
//     // scanf("%d", &n);

//     // // calcul suma:
//     // for (int i = 0; i <= n; i += 2)
//     // {
//     //     sum += i;
//     // }
//     // printf("Suma este: %d\n", sum);
//     init(argc);

//     sem_init(&file_to_write_available, 0, MAX_TASKS);
//     sem_init(&task_to_execute_available, 0, 0);

//     pthread_t tid[MAX_THREADS];

//     for (int i = 0; i < MAX_THREADS; i++)
//     {
//         if (pthread_create(&tid[i], NULL, &starting_routine, NULL) != 0)
//         {
//             perror("Error threads create");
//             exit(-1);
//         }
//     }
//     read_files(argc, argv);

//     for (int i = 0; i < MAX_THREADS; i++)
//     {
//         if (pthread_join(tid[i], NULL) != 0)
//         {
//             perror("Error threads join");
//             exit(-1);
//         }
//     }
//     sem_destroy(&file_to_write_available);
//     sem_destroy(&task_to_execute_available);

//     return 0;
// }
void populate_vector(char *buffer)
{
    char *start = strtok(buffer, "\n");
    while (start != NULL)
    {
        sem_wait(&file_to_write_available);
        Task new_task4;
        new_task4.sum = 0;
        strcpy(new_task4.filename, start);
        printf("Filename found: %s\n", new_task4.filename);
        submit_Task(new_task4);
        start = strtok(NULL, "\n");
        sem_post(&task_to_execute_available);
    }
}
void read_files(int argc, char *argv[])
{

    char filename[1024];
    printf("ARGVs: 0:%s....1: %s...2: %s\n", argv[0], argv[1], argv[2]);

    strcpy(filename, argv[1]);
    printf("FILENAME: %s \n", filename);

    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("open file error");
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    int bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0)
    {
        perror("0 bytes read error");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_read] = '\0';

    printf("Bytes Read: %d\n", bytes_read);
    printf("Data read from file: %s\n", buffer);

    populate_vector(buffer);
    close(fd);
}

void submit_Task(Task new_task)
{
    pthread_mutex_lock(&_task_mutex_);
    task_vector[vector_COUNT__] = new_task;
    vector_COUNT__++;
    pthread_mutex_unlock(&_task_mutex_);
}
void signal_handler(int sig)
{
    printf("Received signal %d. Exiting...\n", sig);
    stop_threads = 1;
    sem_post(&task_to_execute_available);
    sem_destroy(&file_to_write_available);
    sem_destroy(&task_to_execute_available);
    // exit(0);
}
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

    if (argc < 2)
    {
        perror("Argument not given");
        exit(-1);
    }
}

// #define _XOPEN_SOURCE 600
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <pthread.h>
// #include <semaphore.h>
// #include <time.h>
// #include <signal.h>
// #include <string.h>

// #define MAX_TICKETS 5
// #define CLIENTS 7

// typedef struct
// {
//     int tickets[MAX_TICKETS];
//     int start;
//     int end;
//     int count;
// } TicketList;

// TicketList ticket_list = {.start = 0, .end = 0};
// pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
// sem_t space_available;
// sem_t tickets_available;

// void *client_routine(void *args)
// {
//     int delay = rand() % 1000001;
//     usleep(delay);
//     printf("a fost creat threadul cu idul: %ld\n", pthread_self());

//     sem_wait(&tickets_available);
//     pthread_mutex_lock(&list_mutex);

//     int ticket = ticket_list.tickets[ticket_list.start];
//     ticket_list.start++;
//     ticket_list.start = ticket_list.start % MAX_TICKETS;
//     ticket_list.count--;
//     printf("== BILET PRIMIT - TOTAL RAMASE: %d\n", ticket_list.count);

//     pthread_mutex_unlock(&list_mutex);
//     sem_post(&space_available);
// }

// int is_unic(int value)
// {
//     for (int i = ticket_list.start; i < ticket_list.end; i++)
//     {
//         if (ticket_list.tickets[i] == value)
//             return 0;
//     }

//     return 1;
// }

// int generate_ticket()
// {
//     int value = 1000000 + rand() % 1000001;
//     while (!is_unic(value))
//     {
//         value = 1000000 + rand() + 1000001;
//     }

//     return value;
// }

// void handle_sigusr2(int signum)
// {
//     pthread_mutex_lock(&list_mutex);
//     printf("-=-=-BILETE DISPONIBILE: %d\n", ticket_list.count);
//     pthread_mutex_unlock(&list_mutex);
// }

// void child_process(pid_t parent_pid)
// {
//     while (1)
//     {
//         sleep(3);
//         kill(parent_pid, SIGUSR2);
//     }
// }

// int main(int argc, char *argv[])
// {
//     srand(time(NULL));

//     struct sigaction sa_usr2;
//     memset(&sa_usr2, 0, sizeof(sa_usr2));
//     sa_usr2.sa_handler = handle_sigusr2;
//     sigaction(SIGUSR2, &sa_usr2, NULL);

//     sem_init(&space_available, 0, MAX_TICKETS);
//     sem_init(&tickets_available, 0, 0);

//     pid_t pid = fork();
//     if (pid == 0)
//     {

//         child_process(getppid());
//         exit(EXIT_SUCCESS);
//     }

//     pthread_t threads_ids[CLIENTS];
//     for (int i = 0; i < CLIENTS; i++)
//     {
//         pthread_create(&threads_ids[i], NULL, client_routine, NULL);
//     }

//     // generare bilete
//     while (1)
//     {
//         sleep(1);
//         sem_wait(&space_available);
//         pthread_mutex_lock(&list_mutex);

//         int new_ticket = generate_ticket();
//         ticket_list.tickets[ticket_list.end] = new_ticket;
//         ticket_list.end++;
//         ticket_list.end = ticket_list.end % MAX_TICKETS;
//         ticket_list.count++;

//         printf("BILET ADAUGAT - NR TOTAL: %d\n", ticket_list.count);

//         pthread_mutex_unlock(&list_mutex);
//         sem_post(&tickets_available);
//     }

//     for (int i = 0; i < CLIENTS; i++)
//     {
//         pthread_join(threads_ids[i], NULL);
//     }

//     return 0;
// }

// #include <stdio.h>
// #include <unistd.h>
// #include <pthread.h>
// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <semaphore.h>

// #define MAX_THREADS 1000
// #define PORT 8080
// #define MAX_TASKS 2

// int socket_desc, client_sock, client_size;
// struct sockaddr_in server_addr, client_addr;
// char server_message[2000], client_message[2000];
// pthread_mutex_t task_mutex = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t task_cond = PTHREAD_COND_INITIALIZER;
// pthread_cond_t task_Max_cond = PTHREAD_COND_INITIALIZER;
// sem_t semaForr;
// int nr_thread_count;

// typedef struct Task
// {
//     int client_socket;
//     char client_message[2000]
// } Task;

// Task taskQueue[10000];
// int task_count = 0;
// void init_the_connection();
// void respond(const char *message, int cs);
// void handle_request(Task new_Task)
// {
//     // Receive client's message:
//     // We now use client_sock, not socket_desc
//     int recv_nr = recv(new_Task.client_socket, new_Task.client_message, sizeof(new_Task.client_message), 0);
//     if (recv_nr <= 0)
//     {
//         printf("Couldn't receive\n");
//         exit(-1);
//     }
//     new_Task.client_message[recv_nr] = '\0';
//     printf("Msg from client lenght:%d ......: %s\n", recv_nr, new_Task.client_message);
//     respond("SALUTTTTTTT", new_Task.client_socket);
// }

// void *start_routine(void *args)
// {
//     sem_wait(&semaForr);
//     printf("Thread executing....%d\n", nr_thread_count);
//     Task new_task;
//     pthread_mutex_lock(&task_mutex);
//     while (task_count == 0)
//     {
//         pthread_cond_wait(&task_cond, &task_mutex);
//     }

//     // shift all to the right
//     new_task = taskQueue[0];

//     for (int i = 0; i < task_count; i++)
//     {
//         taskQueue[i] = taskQueue[i + 1];
//     }
//     task_count--;
//     // take a task and execute
//     pthread_mutex_unlock(&task_mutex);
//     handle_request(new_task);
//     sem_post(&semaForr);
// }
// int main(int argc, char *argv[])
// {

//     pthread_t thread_poll[MAX_THREADS];
//     sem_init(&semaForr, 0, MAX_THREADS);

//     init_the_connection();

//     int i = 0;
//     while (1)
//     {
//         // Accept an incoming connection from one of the clients :
//         client_size = sizeof(client_addr);
//         client_sock = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);
//         if (client_sock < 0)
//         {
//             printf("Can't accept\n");
//             return -1;
//         }
//         printf("Client: CURRENT TASKS: %d connected at IP: %s and port: %i\n", task_count, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

//         Task new_task;
//         new_task.client_socket = client_sock;
//         pthread_mutex_lock(&task_mutex);
//         taskQueue[task_count] = new_task;
//         task_count++;
//         nr_thread_count++;
//         pthread_mutex_unlock(&task_mutex);
//         pthread_cond_signal(&task_cond);

//         if (pthread_create(&thread_poll[i], NULL, &start_routine, NULL) != 0)
//         {
//             perror("Error at creating thread");
//             exit(-1);
//         }

//         // while (task_count >= MAX_TASKS)
//         // {
//         //     printf("CAN'T ACCEPT THE CONNECTION.....\n");
//         //     pthread_cond_wait(&task_Max_cond, &task_mutex);
//         // }
//     }

//     for (int i = 0; i < MAX_THREADS; i++)
//     {
//         if (pthread_join(thread_poll[i], NULL) != 0)
//         {
//             perror("THreads join error");
//             exit(EXIT_FAILURE);
//         }
//     }

//     // Closing the socket:
//     pthread_cond_destroy(&task_cond);
//     pthread_mutex_destroy(&task_mutex);
//     close(socket_desc);
//     return 0;
// }

// void respond(const char *message, int client_sock_t)
// {
//     // Respond to client:
//     strcpy(server_message, message);

//     if (send(client_sock_t, server_message, strlen(server_message), 0) < 0)
//     {
//         printf("Can't send\n");
//         exit(-1);
//     }
//     close(client_sock_t);
//     pthread_cond_signal(&task_Max_cond);
// }
// void init_the_connection()
// {
//     // Clean buffers:
//     memset(server_message, '\0', sizeof(server_message));
//     memset(client_message, '\0', sizeof(client_message));

//     // Create socket:
//     socket_desc = socket(AF_INET, SOCK_STREAM, 0);

//     if (socket_desc < 0)
//     {
//         printf("Error while creating socket\n");
//         exit(-1);
//     }

//     printf("Socket created successfully\n");

//     // Set port and IP that we'll be listening for, any other IP_SRC or port will be dropped:
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_port = htons(PORT);
//     server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

//     // Bind to the set port and IP:
//     if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
//     {
//         printf("Couldn't bind to the port\n");
//         exit(-1);
//     }
//     printf("Done with binding\n");

//     // Listen for clients:
//     if (listen(socket_desc, 1) < 0)
//     {
//         printf("Error while listening\n");
//         exit(-1);
//     }
//     printf("\nListening for incoming connections.....\n");
// }

#define _XOPEN_SOURCE 600
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#define WRITE_HEAD 1
#define READ_HEAD 0
#define MAX_PTHREADS 30

int line_counter = 0;
char filename_global[BUFSIZ];
sem_t sem;
sem_t *sem_pipe = NULL;
int pipe__[2];
pthread_barrier_t barrier;
pthread_cond_t var_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_list = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_FD_GLOBAL = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_current_TASKS = PTHREAD_MUTEX_INITIALIZER;

int current_tasks = 0;
int list_counter = 0;
typedef struct used_files
{
    int use_flag;
    char filename[BUFSIZ];
} used_files;
used_files files_list[MAX_PTHREADS];
int citit = 0;
FILE *fd_gobal = NULL;
int get_nr_lines(char *filename);
void init(int, char **);

void show_list()
{
    pthread_mutex_lock(&mutex_list);
    for (int i = 0; i < list_counter; i++)
    {
        printf("%s..flag: %d\n", files_list[i].filename, files_list[i].use_flag);
    }
    pthread_mutex_unlock(&mutex_list);
}

int verify_existency(char *filename)
{

    pthread_mutex_lock(&mutex_list);
    for (int i = 0; i < list_counter; i++)
    {
        if (strcmp(files_list[i].filename, filename) == 0)
        {
            pthread_mutex_unlock(&mutex_list);
            return i;
        }
    }
    pthread_mutex_unlock(&mutex_list);
    return 0;
}
void add_in_list(char *filename)
{

    if (verify_existency(filename) == 0)
    {
        // daca nu exista il adaugam
        pthread_mutex_lock(&mutex_list);
        files_list[list_counter].use_flag = 0;
        strcpy(files_list[list_counter].filename, filename);
        list_counter++;
        pthread_mutex_unlock(&mutex_list);
    }
    // daca nu nu il mai adaugam
}
int verify_and_take(char *filename)
{
    int id = verify_existency(filename);
    while (1)
    {
        pthread_mutex_lock(&mutex_list);

        if (files_list[id].use_flag == 0)
        {
            files_list[id].use_flag = 1;
            pthread_mutex_unlock(&mutex_list);

            return id;
        }
        pthread_mutex_unlock(&mutex_list);
    }
}

void generate(char *to_write, char *filename)
{
    pthread_mutex_lock(&mutex_current_TASKS);
    current_tasks++;
    pthread_mutex_unlock(&mutex_current_TASKS);

    int id = verify_and_take(filename);

    int fd = open(filename, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (fd < 0)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    int nr_bytes = write(fd, to_write, strlen(to_write));
    if (nr_bytes < 0)
    {
        perror("error at writeing in the file");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_lock(&mutex_list);
    files_list[id].use_flag = 0;
    pthread_mutex_unlock(&mutex_list);

    pthread_mutex_lock(&mutex_current_TASKS);
    current_tasks--;
    pthread_mutex_unlock(&mutex_current_TASKS);

    close(fd);
}

void checksum(char *filename__1, char *filename__2)
{

    pthread_mutex_lock(&mutex_current_TASKS);
    current_tasks++;
    pthread_mutex_unlock(&mutex_current_TASKS);

    int id1 = verify_and_take(filename__1);
    int id2 = verify_and_take(filename__2);

    int fd1 = open(filename__1, O_RDONLY);
    int fd2 = open(filename__2, O_RDONLY);
    int read_bytes = 0;
    char buffer[BUFSIZ];
    long long sum1 = 0;
    long long sum2 = 0;

    while ((read_bytes = read(fd1, buffer, sizeof(buffer))) > 0)
    {
        for (int i = 0; i < read_bytes; i++)
        {
            sum1 += buffer[i];
        }
        sum1 %= 1000;
    }
    memset(buffer, 0, BUFSIZ);
    while ((read_bytes = read(fd2, buffer, sizeof(buffer))) > 0)
    {
        for (int i = 0; i < read_bytes; i++)
        {
            sum2 += buffer[i];
        }
        sum2 %= 1000;
    }
    pthread_mutex_lock(&mutex_list);
    files_list[id1].use_flag = 0;
    files_list[id2].use_flag = 0;
    pthread_mutex_unlock(&mutex_list);
    // printf("First checksum:%lld...and second...%lld\n", sum1, sum2);
    pthread_mutex_lock(&mutex_current_TASKS);
    current_tasks--;
    pthread_mutex_unlock(&mutex_current_TASKS);
    close(fd1);
    close(fd2);
}

void thread_handler_file(char *buffer)
{
    char operation[BUFSIZ];
    char opp1[BUFSIZ];
    char opp2[BUFSIZ];
    int thread_id = pthread_self() % 100;
    char message[BUFSIZ];
    // sprintf(message, "%d", thread_id);
    // strcat(message, "_START");
    // // printf("Sending the mess: %s by %d\n", message, thread_id);
    // sem_wait(sem_pipe);
    // write(pipe__[WRITE_HEAD], message, strlen(message));
    //
    sem_wait(sem_pipe);
    snprintf(message, sizeof(message), "%d_START\n", thread_id);
    write(pipe__[WRITE_HEAD], message, strlen(message));
    // sem_post(sem_pipe);
    //
    sscanf(buffer, "%s %s %s", operation, opp1, opp2);
    if (strcmp(operation, "generate") == 0)
    {
        add_in_list(opp2);
        generate(opp1, opp2);
    }
    if (strcmp(operation, "checksum") == 0)
    {
        add_in_list(opp2);
        add_in_list(opp1);
        checksum(opp1, opp2);
    }

    memset(&message, 0, sizeof(message));
    // sprintf(message, "%d", thread_id);
    // strcat(message, "_STOP");

    // sem_wait(sem_pipe);
    // printf("Sending the mess: %s by %d\n", message, thread_id);
    // write(pipe__[WRITE_HEAD], message, sizeof(message));
    // pthread_exit(NULL);

    sem_wait(sem_pipe);
    snprintf(message, sizeof(message), "%d_STOP\n", thread_id);
    write(pipe__[WRITE_HEAD], message, strlen(message));
    // sem_post(sem_pipe);
}
void *handle_routine(void *args)
{
    // int ID = *((int *)args);
    //  wait for all threads here
    int rc = pthread_barrier_wait(&barrier);
    if (rc == PTHREAD_BARRIER_SERIAL_THREAD)
    {
        printf("   let the flood in\n");
    }
    char buffer[BUFSIZ];

    while (1)
    {
        sem_wait(&sem);

        if (citit == line_counter)
        {
            sem_post(&sem);
            break;
        }

        // pthread_mutex_lock(&mutex_FD_GLOBAL);
        if (fgets(buffer, sizeof(buffer), fd_gobal) != NULL)
        {
            citit++;
            sem_post(&sem);
            // printf("Thread %ld: %s\n", pthread_self(), buffer);
            thread_handler_file(buffer);
        }
        // pthread_mutex_unlock(&mutex_FD_GLOBAL); // Eliberează mutex-ul
    }

    pthread_exit(NULL);
}
void create_threads()
{
    pthread_t tid[MAX_PTHREADS];
    fd_gobal = fopen(filename_global, "r");
    if (fd_gobal == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Asigură-te că nu depășești dimensiunea maximă
    if (line_counter > MAX_PTHREADS)
    {
        fprintf(stderr, "Too many lines, exceeding maximum thread limit\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < line_counter; i++)
    {
        // Transmiterea unui pointer către fiecare element al vectorului
        pthread_create(&tid[i], NULL, handle_routine, NULL);
    }
    for (int i = 0; i < line_counter; i++)
    {
        pthread_join(tid[i], NULL);
    }
    fclose(fd_gobal);

    pthread_barrier_destroy(&barrier);
}
void handler_sigUSR1(int sig)
{
    pthread_mutex_lock(&mutex_current_TASKS);
    printf("Crrent TASKS in runnig: %d\n", current_tasks);
    pthread_mutex_unlock(&mutex_current_TASKS);
}
void handler_sigint(int sig)
{
    exit(0);
}
void init_signals()
{
    sigset_t set;
    sigemptyset(&set);
    sigfillset(&set);
    sigdelset(&set, SIGUSR1);
    sigdelset(&set, SIGINT);

    struct sigaction sa_int;
    memset(&sa_int, 0, sizeof(sa_int));
    sa_int.sa_handler = handler_sigint;
    sigaction(SIGINT, NULL, &sa_int);

    struct sigaction sa_usr;
    memset(&sa_usr, 0, sizeof(sa_usr));
    sa_usr.sa_handler = handler_sigUSR1;
    sigaction(SIGUSR1, NULL, &sa_usr);
}
void child_routine(int pipe__[2])
{
    close(pipe__[WRITE_HEAD]);

    char message[256];
    int val = 0;
    while (1)
    {
        if (sem_getvalue(sem_pipe, &val) != 0)
        {
            perror("sem_getvalue failed");
            exit(EXIT_FAILURE);
        }
        if (val == 0)
        {
            char buffer[256];
            int bytes_read = read(pipe__[READ_HEAD], buffer, sizeof(buffer) - 1);
            if (bytes_read < 0)
            {
                perror("Error reading from pipe");
                exit(EXIT_FAILURE);
            }
            else if (bytes_read == 0)
            {
                // Pipe închis
                break;
            }
            else
            {
                buffer[bytes_read] = '\0'; // Adăugați terminatorul de șir
                printf("Child received: %s", buffer);

                if (strstr(buffer, "_START") != NULL)
                {
                    int thread_id;
                    sscanf(buffer, "%d_START", &thread_id);
                    printf("Starting timer for thread %d\n", thread_id);
                }
                else if (strstr(buffer, "_STOP") != NULL)
                {
                    int thread_id;
                    sscanf(buffer, "%d_STOP", &thread_id);
                    printf("Stopping timer for thread %d\n", thread_id);
                }
            }
            sem_post(sem_pipe);
        }
    }
}

int main(int argc, char *argv[])
{
    sem_unlink("/sem_pipe");
    init(argc, argv);
    init_signals();
    if (pipe(pipe__) == -1)
    {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }
    //
    //
    pid_t pid = fork();
    if (pid == 0)
    {
        // child
        child_routine(pipe__);
        exit(0);
    }
    else if (pid == -1)
    {
        perror("error init fork");
        exit(EXIT_FAILURE);
    }
    //
    //
    //
    close(pipe__[READ_HEAD]);
    line_counter = get_nr_lines(argv[1]);
    printf("Line counter: %d...\n", line_counter);
    create_threads(line_counter);
    wait(NULL);
    sem_close(sem_pipe);
    sem_unlink("/sem_pipe");
    return 0;
}
void init(int argc, char *argv[])
{
    if (argc <= 0)
    {
        perror("noo enough params to the program");
        exit(EXIT_FAILURE);
    }
    int rd = sem_init(&sem, 0, 1);
    sem_pipe = sem_open("/sem_pipe", O_CREAT | O_RDWR, 0666, 1);
    if (rd < 0)
    {
        perror("errorat creating sem");
        exit(EXIT_FAILURE);
    }
    strcpy(filename_global, argv[1]);
}
int get_nr_lines(char *filename)
{
    // int fd = open(filename, O_RDONLY);
    FILE *fd = fopen(filename, "r");
    if (fd < 0)
    {
        perror("error at opening file");
        exit(EXIT_FAILURE);
    }
    char buffer[BUFSIZ];
    int nr_lines = 0;
    while (fgets(buffer, sizeof(buffer), fd) > 0)
    {
        nr_lines++;
    }
    int rd = pthread_barrier_init(&barrier, NULL, nr_lines);
    if (rd < 0)
    {
        perror("errorat creating barier");
        exit(EXIT_FAILURE);
    }
    fclose(fd);
    return nr_lines;
}
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <pthread.h>
// #include <semaphore.h>
// #include <fcntl.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <time.h>
// #include <signal.h>

// #define MAX_PTHREADS 10
// #define DEADLOCK_TIMEOUT 5 // Timeout de 5 secunde

// // Structura pentru fiecare thread
// typedef struct TimerInfo
// {
//     timer_t timer_id; // ID-ul timerului
//     int thread_id;    // ID-ul threadului
// } TimerInfo;

// TimerInfo active_timers[MAX_PTHREADS]; // Un vector pentru timerele fiecărui thread

// // Pipe pentru comunicarea dintre părinte și copil
// int pipe_fds[2];

// // Functia care va fi apelata atunci când timerul expiră
// void timer_handler(union sigval sv)
// {
//     TimerInfo *info = (TimerInfo *)sv.sival_ptr;
//     printf("Timer expired for thread %d. Deadlock detected!\n", info->thread_id);

//     // Trimiterea semnalului SIGKILL către procesul părinte
//     kill(getppid(), SIGKILL);
// }

// // Pornirea timerului pentru un thread specific
// void start_timer(int thread_id)
// {
//     struct sigevent sev;
//     struct itimerspec its;
//     timer_t timer_id;

//     // Setăm opțiunea să fie apelată o funcție de handler când timerul expiră
//     sev.sigev_notify = SIGEV_THREAD;
//     sev.sigev_notify_function = timer_handler;
//     sev.sigev_notify_attributes = NULL;
//     sev.sigev_value.sival_ptr = (void *)&(active_timers[thread_id]);

//     // Creăm timerul
//     if (timer_create(CLOCK_REALTIME, &sev, &timer_id) == -1)
//     {
//         perror("Error creating timer");
//         exit(EXIT_FAILURE);
//     }

//     // Setăm timerul să expire în DEADLOCK_TIMEOUT (5 secunde)
//     its.it_value.tv_sec = DEADLOCK_TIMEOUT;
//     its.it_value.tv_nsec = 0;
//     its.it_interval.tv_sec = 0; // Nu vrem timerul să se repete
//     its.it_interval.tv_nsec = 0;

//     // Pornim timerul
//     if (timer_settime(timer_id, 0, &its, NULL) == -1)
//     {
//         perror("Error starting timer");
//         exit(EXIT_FAILURE);
//     }

//     active_timers[thread_id].timer_id = timer_id; // Păstrăm ID-ul timerului
//     active_timers[thread_id].thread_id = thread_id;

//     printf("Started timer for thread %d\n", thread_id);
// }

// // Oprirea timerului pentru un thread specific
// void stop_timer(int thread_id)
// {
//     printf("Stopping timer for thread %d\n", thread_id);

//     // Anulăm timerul folosind timer_delete
//     if (timer_delete(active_timers[thread_id].timer_id) == -1)
//     {
//         perror("Error deleting timer");
//         exit(EXIT_FAILURE);
//     }

//     printf("Timer stopped for thread %d\n", thread_id);
// }

// // Funcția principală a thread-ului care va lucra cu fișierul
// void *worker_thread(void *arg)
// {
//     int thread_id = *(int *)arg;
//     char buffer[256];

//     // La începutul execuției, trimitem ID_START și pornim timerul
//     printf("Thread %d: Starting task\n", thread_id);
//     snprintf(buffer, sizeof(buffer), "%d_START", thread_id);
//     write(pipe_fds[1], buffer, strlen(buffer) + 1); // Trimite mesajul prin pipe
//     printf("Sent message: %s\n", buffer);
//     start_timer(thread_id); // Pornește timerul pentru acest thread

//     // Simulăm lucrarea thread-ului
//     sleep(2); // Execută ceva muncă

//     // După ce lucrul este terminat, trimitem ID_STOP și oprim timerul
//     snprintf(buffer, sizeof(buffer), "%d_STOP", thread_id);
//     write(pipe_fds[1], buffer, strlen(buffer) + 1); // Trimite mesajul prin pipe
//     printf("Sent message: %s\n", buffer);
//     stop_timer(thread_id); // Oprește timerul

//     // Termina executia
//     pthread_exit(NULL);
// }

// // Procesul copil care va monitoriza semnalele
// void child_process(int pipe_fd[2])
// {
//     char message[256];
//     while (1)
//     {
//         // Citim mesajul din pipe
//         int bytes_read = read(pipe_fd[0], message, sizeof(message));
//         if (bytes_read < 0)
//         {
//             perror("Error reading from pipe");
//             exit(EXIT_FAILURE);
//         }

//         // Procesăm mesajul primit
//         printf("Child received: %s\n", message);

//         // Dacă mesajul conține "_START", pornește timerul
//         if (strstr(message, "_START") != NULL)
//         {
//             int thread_id;
//             sscanf(message, "%d_START", &thread_id);
//             printf("Starting timer for thread %d\n", thread_id);
//             start_timer(thread_id);
//         }
//         // Dacă mesajul conține "_STOP", oprește timerul
//         else if (strstr(message, "_STOP") != NULL)
//         {
//             int thread_id;
//             sscanf(message, "%d_STOP", &thread_id);
//             printf("Stopping timer for thread %d\n", thread_id);
//             stop_timer(thread_id);
//         }
//     }
// }

// // Funcția principală
// int main()
// {
//     // Creăm pipe-ul pentru comunicare între părinte și copil
//     if (pipe(pipe_fds) == -1)
//     {
//         perror("Error creating pipe");
//         exit(EXIT_FAILURE);
//     }

//     // Creăm procesul copil
//     pid_t child_pid = fork();
//     if (child_pid == -1)
//     {
//         perror("Fork failed");
//         exit(EXIT_FAILURE);
//     }

//     if (child_pid == 0)
//     {
//         // Codul procesului copil
//         close(pipe_fds[1]);
//         child_process(pipe_fds); // Procesul copil monitorizează mesajele și gestionează timerele
//         exit(EXIT_SUCCESS);
//     }
//     else
//     {
//         // Codul procesului părinte
//         close(pipe_fds[0]);

//         pthread_t threads[MAX_PTHREADS];
//         int thread_ids[MAX_PTHREADS];

//         // Creăm thread-urile
//         for (int i = 0; i < MAX_PTHREADS; i++)
//         {
//             thread_ids[i] = i + 1;
//             pthread_create(&threads[i], NULL, worker_thread, &thread_ids[i]);
//         }

//         // Așteptăm ca toate thread-urile să termine
//         for (int i = 0; i < MAX_PTHREADS; i++)
//         {
//             pthread_join(threads[i], NULL);
//         }

//         // Așteaptă terminarea procesului copil
//         wait(NULL);
//     }

//     return 0;
// }

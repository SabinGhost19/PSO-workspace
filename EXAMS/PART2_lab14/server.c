#define _GNU_SOURCE
#include <mqueue.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/wait.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/sendfile.h>
#include <pthread.h>
#define STATUS_RUNNING 0
#define STATUS_FINISHED 1
#define SEED_INIT 123456


int id_seed = SEED_INIT;

typedef struct job
{
  int pid;
  int status;
  char command[128];
} job_t;

sem_t sem;
job_t jobs[1000];
pthread_mutex_t jobs_mutex = PTHREAD_MUTEX_INITIALIZER;
int running_jobs_nr = 0;

pthread_mutex_t job_index_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t finish_mutex = PTHREAD_MUTEX_INITIALIZER;

int finished = 0;

void signal_handler(int signal)
{
  pthread_mutex_lock(&finish_mutex);
  finished = 1;
  pthread_mutex_unlock(&finish_mutex);
}

void set_signal()
{
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));

  sa.sa_handler = signal_handler;
  sigaction(SIGTERM, &sa, NULL);
}

void run(int id, char *command)
{
  char *saveptr;
  char *args[10];

  // pthread_mutex_lock(&jobs_mutex);
  strcpy(jobs[id - SEED_INIT].command, command);
  jobs[id - SEED_INIT].status = STATUS_RUNNING;
  char *p = strtok_r(command, " \n", &saveptr);
  // pthread_mutex_unlock(&jobs_mutex);

  int i = 0;

  // Am aduagat si suport pentru scripturi
  if (strcmp(command + strlen(command) - 3, ".sh") == 0) {
    args[0] = "/bin/bash";
    command = args[0];
    i++;
  }

  for (; p != NULL; i++)
  {
    args[i] = p;
    p = strtok_r(NULL, " \n", &saveptr);
  }
  args[i] = NULL;
  pid_t pid = fork();

  if (pid == 0)
  {
    char filename_out[256], filename_err[256];
    sprintf(filename_out, "%d.stdout", id);
    sprintf(filename_err, "%d.stderr", id);
    int out = open(filename_out, O_CREAT | O_WRONLY, 0664);
    int err = open(filename_err, O_CREAT | O_WRONLY, 0664);


    dup2(out, STDOUT_FILENO);
    dup2(err, STDERR_FILENO);

    execvp(command, args);
    perror("exec");
    exit(-1);
    // execlp(args, args, NULL);
  }
  else if (pid > 0)
  {
    pthread_mutex_lock(&jobs_mutex);
    jobs[id - SEED_INIT].pid = pid;
    pthread_mutex_unlock(&jobs_mutex);
    sem_post(&sem);
    printf("job run!\n");
    return;
  }
  else
  {
    perror("fork");
  }
}

pthread_mutex_t id_seed_mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread_handle(void *params)
{
  int client = *((int *)params);
  char buffer[1024];
  
  recv(client, buffer, 1024, 0);
  printf("%s\n", buffer);
  
  char *command, *args = NULL;

  // am adaugat aici verificarea ca am un spatiu, altfel nu am parametru
  char* first_space = strchr(buffer, ' ');
  if (first_space != NULL) {
    args = strchr(buffer, ' ') + 1;
    buffer[strchr(buffer, ' ') - buffer] = '\0';
  } 
  command = buffer;

  if (strcmp("run", command)==0)
  {
    pthread_mutex_lock(&id_seed_mutex);
    int id = id_seed++;
    pthread_mutex_unlock(&id_seed_mutex); 

    pthread_mutex_lock(&job_index_mutex);
    if (running_jobs_nr >= 5)
    {
      pthread_mutex_unlock(&job_index_mutex);
      char error = -1;
      write(client, &error, 1);
      write(client, "Error! Job queue is full", 18);
      return NULL;
    }
    else
    {
      running_jobs_nr++;
      pthread_mutex_unlock(&job_index_mutex);

      run(id, args);

      char error = 0;
      write(client, &error, 1);
      write(client, &id, sizeof(id));
    }
  }

  if (strcmp("get_status", command)==0)
  {
    int id = atoi(args);
    pthread_mutex_lock(&jobs_mutex);
    if (jobs[id - SEED_INIT].pid != 0)
    {
      pthread_mutex_unlock(&jobs_mutex);

      char error = 0;
      write(client, &error, 1);
      write(client, &jobs[id - SEED_INIT].status, 4);
    }
    else
    {
      pthread_mutex_lock(&jobs_mutex);
      char error = -1;
      write(client, &error, 1);
    }
  }

  if (strcmp("kill_job", command)==0)
  {
    int id = atoi(args);
    pthread_mutex_lock(&jobs_mutex);
    if (jobs[id - SEED_INIT].pid != 0 && jobs[id - SEED_INIT].status == STATUS_RUNNING)
    {
      kill(jobs[id - SEED_INIT].pid, SIGKILL);
      jobs[id - SEED_INIT].status = STATUS_FINISHED;
    }
    pthread_mutex_unlock(&jobs_mutex);
    char error = 0;
    write(client, &error, 1);
  }

  if (strcmp("get_stdout", command)==0)
  {
    struct stat file_stat;
    int id = atoi(args);

    pthread_mutex_lock(&jobs_mutex);
    if (jobs[id - SEED_INIT].pid != 0 && jobs[id - SEED_INIT].status == STATUS_FINISHED)
    {
      pthread_mutex_unlock(&jobs_mutex);

      char buffer[128];
      sprintf(buffer, "%d.stdout", id);

      int fd = open(buffer, O_RDONLY);
      fstat(fd, &file_stat);
      char error = 0;
      write(client, &error, 1);
      sendfile(client, fd, 0, file_stat.st_size);
    }
    else
    {
      pthread_mutex_unlock(&jobs_mutex);
      char error = -1;
      write(client, &error, 1);
    }
  }

  if (strcmp("get_stderr", command)==0)
  {
    struct stat file_stat;
    int id = atoi(args);

    pthread_mutex_lock(&jobs_mutex);
    if (jobs[id - SEED_INIT].pid != 0 && jobs[id - SEED_INIT].status == STATUS_FINISHED)
    {
      pthread_mutex_unlock(&jobs_mutex);

      char buffer[128];
      sprintf(buffer, "%d.stderr", id);

      int fd = open(buffer, O_RDONLY);
      fstat(fd, &file_stat);
      char error = 0;
      write(client, &error, 1);
      sendfile(client, fd, 0, file_stat.st_size);
    }
    else
    {
      pthread_mutex_unlock(&jobs_mutex);
      char error = -1;
      write(client, &error, 1);
    }
  }

  if (strcmp("get_jobs", command)==0)
  {
    int nr;
    pthread_mutex_lock(&id_seed_mutex);
    nr = id_seed - SEED_INIT - 1;
    pthread_mutex_unlock(&id_seed_mutex);
    char error = 0;
    write(client, &error, 1);
    write(client, &nr, 4);

    char buffer[256];

    pthread_mutex_lock(&jobs_mutex);
    for (int i = 0; i < nr; i++)
    {
      if (jobs[i].pid != 0 && jobs[i].status == STATUS_RUNNING)
      {
        sprintf(buffer, "%d %s", i + SEED_INIT, jobs[i].command);
        write(client, buffer, 128);
      }
    }
    pthread_mutex_unlock(&jobs_mutex);
  }
  
  if (strcmp("istoric", command)==0)
  {
    int nr;
    pthread_mutex_lock(&id_seed_mutex);
    nr = id_seed - SEED_INIT;
    pthread_mutex_unlock(&id_seed_mutex);
    char error = 0;
    write(client, &error, 1);
    write(client, &nr, 4);

    char buffer[256];

    pthread_mutex_lock(&jobs_mutex);
    for (int i = 0; i < nr; i++)
    {
      if (jobs[i].pid != 0 && jobs[i].status == STATUS_FINISHED)
      {
        sprintf(buffer, "%d %s", i + SEED_INIT, jobs[i].command);
        write(client, buffer, 128);
      }
    }
    pthread_mutex_unlock(&jobs_mutex);
  }

  close(client);
  return NULL;
}

void *thread_handle_wait(void *p)
{
  pthread_mutex_lock(&finish_mutex);
  while (!finished)
  {
    pthread_mutex_unlock(&finish_mutex);

    sem_wait(&sem);
    int pid = wait(NULL);
    int nr;
    pthread_mutex_lock(&id_seed_mutex);
    nr = id_seed - SEED_INIT;
    pthread_mutex_unlock(&id_seed_mutex);

    if (pid != -1)
    {
      for (int i = 0; i < nr; i++)
      {
        if (jobs[i].pid == pid)
        {
          pthread_mutex_lock(&jobs_mutex);
          jobs[i].status = STATUS_FINISHED;
          pthread_mutex_unlock(&jobs_mutex);

          break;
        }
      }
    }
    pthread_mutex_lock(&job_index_mutex);
    running_jobs_nr--;
    pthread_mutex_unlock(&job_index_mutex);
    pthread_mutex_lock(&finish_mutex);
  }
  return NULL;
}

int main(int argc, char **argv)
{

  set_signal();
  sem_init(&sem, 1, 0);

  int socket_desc;
  unsigned int client_size;
  struct sockaddr_in server_addr, client_addr;

  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_desc < 0)
  {
    printf("Error while creating socket\n");
    return -1;
  }
  printf("Socket created successfully\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(2000);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  const int enable = 1;
  if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    perror("setsockopt(SO_REUSEADDR) failed");

  // Bind to the set port and IP:
  if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    printf("Couldn't bind to the port\n");
    return -1;
  }
  printf("Done with binding\n");

  // Listen for clients:
  if (listen(socket_desc, 1) < 0)
  {
    printf("Error while listening\n");
    return -1;
  }

  // TODO: Synchronize thread resouces
  pthread_t tid[500];
  int param[500];


  pthread_t w_tid;
  pthread_create(&w_tid, NULL, thread_handle_wait, NULL);

  int current = 0;
  int retval;
  fd_set rfds;
  struct timeval tv;

  pthread_mutex_lock(&finish_mutex);
  while (!finished)
  {
    pthread_mutex_unlock(&finish_mutex);
    client_size = sizeof(client_addr);
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    FD_ZERO(&rfds);
    FD_SET(socket_desc, &rfds);
    retval = select(socket_desc + 1 , &rfds, NULL, NULL, &tv);
    if (retval == -1)
      perror("select()");
    else if (retval) {
      printf("accept\n");
        param[current] = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);
        
        // sa nu poata fi mostenit de copii (vezi si accept4())
        fcntl(param[current], F_SETFD, fcntl(param[current], F_GETFD) | FD_CLOEXEC);

        pthread_create(&tid[current], NULL, thread_handle, &param[current]);
        pthread_mutex_lock(&finish_mutex);
        current++;
      }
  }

  for (int i = 0; i < current; i++)
  {
    pthread_join(tid[i], NULL);
  }

  pthread_join(w_tid, NULL);

  return 0;
}

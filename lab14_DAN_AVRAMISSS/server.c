#define _GNU_SOURCE
#include <mqueue.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>

#define STATUS_SIZE 20
#define JOB_SIZE 100000
#define JOB_NOT_EXIST "JOB NOT EXIST"
#define JOB_LIMIT "TO MANY JOBS...PLEASE WAIT"
#define MAX_CONNECTIONS 10000
#define MAX_THREADS 100

int job_index = 0;
int id_seed = 100;
int running_jobs = 0;
typedef struct job
{
  char status[STATUS_SIZE];
  int job_id;
} job;

job jobs[JOB_SIZE];

pthread_mutex_t job_index_mutex = PTHREAD_MUTEX_INITIALIZER;

#define BUF_SIZE (1 << 13)
#define NAME "/test_queue"

char buf[BUF_SIZE];

void signal_handler(int signal)
{
}

void set_signal()
{
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));

  sa.sa_handler = signal_handler;
  sigaction(SIGTERM, &sa, NULL);
}

void run(int id, char *args, int client)
{
  pid_t pid = fork();

  if (pid == 0)
  {
    char filename_out[256], filename_err[256];
    sprintf(filename_out, "%d.stdout", id);
    sprintf(filename_err, "%d.stderr", id);

    int out = open(filename_out, O_CREAT | O_WRONLY | O_TRUNC, 0664);
    int err = open(filename_err, O_CREAT | O_WRONLY | O_TRUNC, 0664);

    if (out == -1 || err == -1)
    {
      perror("open");
      exit(EXIT_FAILURE);
    }

    if (dup2(out, STDOUT_FILENO) == -1)
    {
      perror("dup2 STDOUT");
      exit(EXIT_FAILURE);
    }
    if (dup2(err, STDERR_FILENO) == -1)
    {
      perror("dup2 STDERR");
      exit(EXIT_FAILURE);
    }

    close(out);
    close(err);

    char args_dot[BUF_SIZE];
    sprintf(args_dot, "./%s", args);
    if (execlp(args_dot, NULL, (char *)NULL) == -1)
    {
      perror("execvp failed");
      exit(EXIT_FAILURE);
    }
  }
  else if (pid > 0)
  {
    pthread_mutex_lock(&job_index_mutex);
    jobs[id].job_id = id;
    strcpy(jobs[id].status, "RUNNING");
    running_jobs++;
    id_seed++;
    pthread_mutex_unlock(&job_index_mutex);

    printf("Parent process: Waiting for child to finish...\n");

    // WRITE TO CLIENT
    // after executing...
    // give the id
    write(client, &id, sizeof(id));

    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status))
    {
      printf("Child exited normally with exit status %d\n", WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
    {
      printf("Child terminated by signal %d\n", WTERMSIG(status));
    }
    else if (WIFSTOPPED(status))
    {
      printf("Child stopped by signal %d\n", WSTOPSIG(status));
    }
    else
    {
      printf("Child process ended in an unknown way.\n");
    }
    pthread_mutex_lock(&job_index_mutex);
    strcpy(jobs[id].status, "TERMINATED");
    running_jobs--;
    job_index++;
    pthread_mutex_unlock(&job_index_mutex);
    // return
  }
  else
  {
    perror("fork");
  }
}
char *get_status(int id)
{
  if (id < 100 || id > id_seed)
  {
    return JOB_NOT_EXIST;
  }
  return jobs[id].status;
}

void get_stdout(int id, int client)
{
  char filename_out[256];
  char char_id[BUF_SIZE];
  int id_copy = id;
  int size = 0;
  while (id_copy != 0)
  {
    id_copy /= 10;

    size++;
  }
  size--;
  int copy_size = size;
  while (size != -1)
  {
    int cifra = id % 10;
    char_id[size] = cifra + '0';
    id /= 10;
    size--;
  }
  char_id[copy_size + 1] = '\0';

  sprintf(filename_out, "%s.stdout", char_id);
  printf("FILENAME: %s\n", filename_out);

  int out = open(filename_out, O_RDONLY);
  if (out == -1)
  {
    perror("Failed to open file");
  }

  char buffer[BUF_SIZE];
  int rc = read(out, buffer, sizeof(buffer));

  if (rc == -1)
  {
    perror("Failed to read from file");
    close(out);
  }

  printf("Read %d bytes from file.\n", rc);

  write(client, buffer, strlen(buffer) + 1);

  close(out);
}

void *thread_handle(void *params)
{
  int client = *((int *)params);
  char buffer[1024];
  int id = id_seed;

  recv(client, buffer, 1024, 0);
  char *command, *args;

  args = strchr(buffer, ' ') + 1;
  buffer[strchr(buffer, ' ') - buffer] = '\0';
  command = buffer;

  printf("Command: %s\n", command);

  if (strcmp("run", command) == 0)
  {
    printf("RUNNING: %d\n", running_jobs);
    if (running_jobs >= 5)
    {
      int args_int = 0;
      write(client, &args_int, sizeof(args_int));
    }
    else
    {
      run(id, args, client);
    }
  }

  if (strcmp("get_status", command) == 0)
  {
    int args_int;
    args_int = strtol(args, NULL, 10);

    char *status = get_status(args_int);
    write(client, status, strlen(status) + 1);
  }

  if (strcmp("get_stdout", command) == 0)
  {
    int args_int;
    args_int = strtol(args, NULL, 10);
    get_stdout(args_int, client);
  }

  // TODO:
  //  if (strcmp("kill_job", command))
  //  {
  //    kill_job(id);
  //  }
  //  if (strcmp("get_stderr", command))
  //  {
  //    get_stderr(id);
  //  }

  // if (strcmp("get_jobs", command))
  // {
  // }
  // if (strcmp("istoric", command))
  // {
  // }

  return NULL;
}

int main(int argc, char **argv)
{

  set_signal();

  int socket_desc, client_size;
  struct sockaddr_in server_addr, client_addr;

  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_desc < 0)
  {
    printf("Error while creating socket\n");
    return -1;
  }
  printf("Socket created successfully\n");

  // Set port and IP that we'll be listening for, any other IP_SRC or port will be dropped:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(2005);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

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
  printf("\nListening for incoming connections.....\n");

  // TODO: Synchronize thread resouces

  pthread_t tid[MAX_THREADS];
  int client_sock_fd[MAX_CONNECTIONS];

  int current = 0;
  while (1)
  {
    client_size = sizeof(client_addr);
    client_sock_fd[current] = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);

    // maximum 5
    pthread_create(&tid[current], NULL, thread_handle, &client_sock_fd[current]);
    current++;
  }

  return 0;
}

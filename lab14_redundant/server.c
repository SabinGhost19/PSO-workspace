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

int jobs[100];
int job_index = 0;
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

void run(int id, char *args)
{
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

    execlp(args, args, NULL);
  }
  else if (pid > 0)
  {
    pthread_mutex_lock(&job_index_mutex);
    jobs[job_index++] = id;
    pthread_mutex_unlock(&job_index_mutex);
    return
  }
  else
  {
    perror("fork");
  }
}

int id_seed = 123456;
void *thread_handle(void *params)
{
  int client = *((int *)params);
  char buffer[1024];
  int id = id_seed++;

  recv(client, buffer, 1024, 0);
  char *command, *args;

  args = strchr(buffer, ' ') + 1;
  buffer[strchr(buffer, ' ') - buffer] = '\0';
  command = buffer;

  if (strcmp("run", command))
  {
    run(id, args);
    write(client, &id, sizeof(job_id));
  }

  if (strcmp("get_status", command))
  {
    get_status(id);
  }
  if (strcmp("kill_job", command))
  {
    kill_job(id);
  }
  if (strcmp("get_stdout", command))
  {
    get_stdout(id);
  }
  if (strcmp("get_stderr", command))
  {
    get_stderr(id);
  }

  if (strcmp("get_jobs", command))
  {
  }
  if (strcmp("istoric", command))
  {
  }

  return NULL;
}

int main(int argc, char **argv)
{

  set_signal();

  int socket_desc, client_sock, client_size;
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
  server_addr.sin_port = htons(2000);
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
  pthread_t tid[5];
  int param[5];

  int current = 0;
  while (1)
  {
    client_size = sizeof(client_addr);
    param[current] = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);

    // maximum 5
    pthread_create(&tid[current], NULL, thread_handle, &param[current]);
  }

  return 0;
}

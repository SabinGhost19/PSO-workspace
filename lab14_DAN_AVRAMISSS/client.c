#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/* Set buffer size at least the default maxim size of the queue
 * found in/proc/sys/kernel/msgmax */
#define BUF_SIZE (1 << 13)
#define TEXT "test message"
#define NAME "/test_queue"
#define JOB_LIMIT "TO MANY JOBS...PLEASE WAIT"

char buf[BUF_SIZE];
char server_message[2000];

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    printf("Command not specified...\n");
    exit(-1);
  }

  int socket_desc;
  struct sockaddr_in server_addr;

  // Create socket, we use SOCK_STREAM for TCP
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_desc < 0)
  {
    printf("Unable to create socket\n");
    return -1;
  }

  printf("Socket created successfully\n");

  // Set port and IP the same as server-side:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(2005);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Send connection request to server:
  if (connect(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    printf("Unable to connect\n");
    return -1;
  }
  printf("Connected with server successfully\n");

  // Get input from the user:
  printf("Enter message: ");

  char command[BUF_SIZE] = "";
  for (int i = 1; i < argc; i++)
  {
    strcat(command, argv[i]);
    if (i < argc - 1)
    {
      strcat(command, " "); // Adaugă un spațiu între argumente
    }
  }

  // Send the constructed command to the server
  printf("Sending command: %s\n", command);
  if (strcmp(argv[1], "run") == 0)
  {
    if (send(socket_desc, command, strlen(command) + 1, 0) < 0)
    {
      printf("Unable to send message\n");
      return -1;
    }

    int server_int = 0;
    if (recv(socket_desc, &server_int, sizeof(server_int), 0) < 0)
    {
      printf("Error while receiving server's msg\n");
      return -1;
    }
    if (server_int == 0)
    {
      printf("%s\n", JOB_LIMIT);
    }
    else
    {
      printf("Server's response: %d\n", server_int);
    }
  }
  if (strcmp(argv[1], "get_status") == 0)
  {
    if (send(socket_desc, command, strlen(command) + 1, 0) < 0)
    {
      printf("Unable to send message\n");
      return -1;
    }

    char job_status[BUF_SIZE];
    if (recv(socket_desc, &job_status, sizeof(job_status), 0) < 0)
    {
      printf("Error while receiving server's msg\n");
      return -1;
    }

    printf("Server's response_STATUS: %s\n", job_status);
  }
  if (strcmp(argv[1], "get_stdout") == 0)
  {
    if (send(socket_desc, command, strlen(command) + 1, 0) < 0)
    {
      printf("Unable to send message\n");
      return -1;
    }

    char job_STDOUT[BUF_SIZE];
    if (recv(socket_desc, &job_STDOUT, sizeof(job_STDOUT), 0) < 0)
    {
      printf("Error while receiving server's msg\n");
      return -1;
    }

    printf("Server's response_STDOUT: %s\n", job_STDOUT);
  }
  // // Close the socket:
  // close(socket_desc);

  return 0;
}

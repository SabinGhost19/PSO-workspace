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

char buf[BUF_SIZE];

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
  server_addr.sin_port = htons(2000);
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

  // Send the message to server:
  if (send(socket_desc, argv[1], strlen(argv[1]) + 1, 0) < 0)
  {
    printf("Unable to send message\n");
    return -1;
  }

  return 0;
}

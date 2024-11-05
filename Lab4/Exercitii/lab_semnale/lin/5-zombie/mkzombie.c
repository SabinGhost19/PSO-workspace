/**
 * SO
 * Lab #4
 *
 * Task #5, Linux
 *
 * Creating zombies
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "utils.h"

#define TIMEOUT 20

int main(void)
{
	pid_t pid;

	/* TODO - create child process without waiting */

	/* TODO - sleep */
	switch (pid = fork())
	{
	case -1:
	{
		perror("Fork error");
		exit(EXIT_FAILURE);
	}
	case 0:
	{
		// child
		printf("Child proc\n");
		printf("PID: %ld", (long)getpid());
		exit(0);
	}
	default:
	{
		// parent
		sleep(TIMEOUT);
	}
	}

	return 0;
}

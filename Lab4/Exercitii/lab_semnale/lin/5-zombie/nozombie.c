/**
 * SO
 * Lab #4
 *
 * Task #5, Linux
 *
 * Avoid creating zombies using signals
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

#define TIMEOUT 20

/*
 * configure signal handler
 */
static void set_signals(void)
{
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);

	sa.sa_flags = 0;
	sigaction(SIGCHLD, &sa, NULL);

	/* TODO - ignore SIGCHLD */
}

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
		printf("Child: %ld \n", (long)getpid());
		sleep(2);
		exit(0);
	}
	default:
	{
		// parent
		set_signals();

		sleep(20);
	}
	}
	return 0;
}

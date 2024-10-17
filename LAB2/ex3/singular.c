#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h> /* flock */
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h> /* errno */
#define LOCK_FILE "/tmp/my_lock_file"

static int fdlock = -1;

static void do_stuff(void)
{
	sleep(10);
}

static void check_lock(void)
{
	int rc;

	/* TODO - Open LOCK_FILE file */
	/* fdlock = open(...) */
	fdlock = open(LOCK_FILE, O_CREAT | O_EXCL | O_WRONLY, 0666);

	if (flock(fdlock, LOCK_EX) < 0)
	{
		perror("there is another instance running");
		exit(EXIT_FAILURE);
	}

	/**
	 * TODO - Lock the file using flock
	 * - flock must not block in any case !
	 *
	 * - in case of error - print a message showing
	 *   there is another instance running and exit
	 */

	printf("\nGot Lock\n\n");
}

static void clean_up(void)
{
	int rc;
	flock(fdlock, LOCK_UN);
	close(fdlock);
	unlink(LOCK_FILE);
	/* TODO - Unlock file, close file and delete it */
}

int main(void)
{
	check_lock();
	do_stuff();
	clean_up();
	return 0;
}

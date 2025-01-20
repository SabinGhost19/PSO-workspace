#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
// #include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
//!!! careful for the sender
#define BUFF_SIZE (1 << 13)
#define TEXT "test_message"
#define NAME "/test_queue"

// char getch(void)
// {
//         char buf = 0;
//         struct termios old = {0};
//         fflush(stdout);
//         if (tcgetattr(0, &old) < 0)
//                 perror("tcsetattr()");

//         old.c_lflag &= ~ICANON;
//         old.c_lflag &= ~ECHO;
//         old.c_cc[VMIN] = 1;
//         old.c_cc[VTIME] = 0;
//         if (tcsetattr(0, TCSANOW, &old) < 0)
//                 perror("tcsetattr ICANON");
//         if (read(0, &buf, 1) < 0)
//                 perror("read()");
//         old.c_lflag |= ICANON;
//         old.c_lflag |= ECHO;
//         if (tcsetattr(0, TCSADRAIN, &old) < 0)
//                 perror("tcsetattr ~ICANON");
//         return buf;
// }
int main(int argc, char *argv[])
{
        unsigned int prio = 10;
        mqd_t m;
        int rc;
        int nr_of_chars = 0;
        m = mq_open(NAME, O_CREAT | O_RDWR, 0666, NULL);
        char buffer[20];

        while (nr_of_chars < 20)
        {
                if (read(0, &buffer, 1) < 0)
                {
                        perror("read()");
                }
                rc = mq_send(m, buffer, strlen(buffer), prio);
                nr_of_chars++;
        }
        // just close , let the receiver to unlink the mq
        mq_close(m);
        return 0;
}
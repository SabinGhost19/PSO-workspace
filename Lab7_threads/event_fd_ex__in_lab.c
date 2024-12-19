#include <stdio.h>
#include <sys/eventfd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int efd;
    uint64_t w = 1;
    uint64_t r = 0;

    // count init with val spec in init_val
    // zice de cate ori a aparut evenimentul
    efd = eventfd(0, 0);

    switch (fork())
    {
    case 0:
    {
        // semnalare event
        //  specific 2 param un nr pe care il va adauga in counter de event
        //  ex : 1 -> semnalam ca a aparut o sing in stanta de event
        // il incrementeaza
        sleep(3);
        write(efd, &w, sizeof(uint64_t));
    }
    case -1:
    {
    }
    default:
    {
        read(efd, &r, sizeof(uint64_t));
        printf("from child we received: %llu\n", r);
    }
    }
    return 0;
}
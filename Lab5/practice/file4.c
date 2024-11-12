#include <mcheck.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    mtrace();

    char *ptr = (char *)malloc(100);

    muntrace();

    return 0;
}

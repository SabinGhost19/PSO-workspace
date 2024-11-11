#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include <stddef.h>
#define GB 1000000
int main()
{

    int *ptr = malloc(GB);
    ptr[0] = 9;
    printf("%4d\n\n", *ptr);
    ptr--;
    ptr--;
    ptr--;

    printf("f: %d s: %d th: %d nouale:%d\n\n", ptr[0], ptr[1], ptr[2], ptr[3]);
    printf("%ld", INT_MAX);
    return 0;
}
#include <stdlib.h>
#include <stdio.h>

int main()
{

    void *ptr1 = malloc(10);
    void *ptr2 = malloc(20);

    free(ptr1);
    void *ptr3 = malloc(2);

    printf("PTR2: %p\n\n", ptr2);
    printf("PTR3: %p", ptr3);

    return 0;
}

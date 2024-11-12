#include <stdio.h>
#include <stdlib.h>

static int fibonacci(int no)
{
    if (1 == no || 2 == no)
        return 1;
    // printf("%d....%d\n", no - 1, no - 2);
    return fibonacci(no - 1) + fibonacci(no - 2);
}

int main(void)
{
    short int numar, baza = 10;
    char sir[1];

    printf("Introduceti numarul: ");
    scanf("%s", sir);
    numar = strtol(sir, NULL, baza);
    printf("\nsir: %s ...numnar:%d ...baza: %d ", sir, numar, baza);
    // printf("fibonacci(%d)=%d\n", numar, fibonacci(numar));
    fibonacci(0);
    fibonacci(1);
    fibonacci(2);
    fibonacci(3);
    fibonacci(4);
    fibonacci(5);
    char *ptr = NULL;
    printf("%c", *ptr);
    // IT DOESNT BRAKEE.....
    return 0;
}
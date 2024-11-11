#include <stdlib.h>
#include <stdio.h>

int main()
{

    char buffer[2];
    int number = 12345;
    scanf("%s", &buffer);
    printf("%s and the int: %d", buffer, number);

    return 0;
}
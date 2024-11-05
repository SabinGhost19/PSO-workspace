#include <stdio.h>
#include <stdlib.h>

void conv(u_int8_t *arr, size_t size)
{
    u_int8_t mask = 0x01;

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            mask = 0x01;
            mask = mask << j;
            mask &= arr[i];
            mask = mask >> j;
            if (mask == 0)
            {
                printf("0");
            }
            else
            {
                printf("1");
            }
        }
        printf("  ");
    }
}
int main()
{

    int8_t arr[] = {0xAB, 0x10, 0x05, 0x03, 0x09};
    conv(arr, sizeof(u_int8_t) * 5);
}
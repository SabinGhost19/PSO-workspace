#include <stdio.h>
 
#define NAME "student"
#define NR (10)

void my_asm_func(int);

 
int main()
{
  int i;
    
    my_asm_func(NR);
    printf("\n");
  // print the NAME NR times
  for (i = 0; i < NR; i++)
    printf("Hello, %s\n", NAME);
 
  return 0;
}
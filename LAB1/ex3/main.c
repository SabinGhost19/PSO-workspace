#include "mymath.h"
#include <stdio.h>
 
int main()
{
  int x, y;
  printf("Introduceti doua numere\n");
  scanf("%d%d", &x, &y);
 
  printf("%d + %d = %d\n", x, y, add(x, y));
  printf("%d - %d = %d\n", x, y, sub(x, y));
 
  return 0;
}
#include <stdio.h>
#include <string.h>
#include <stdio.h>

const char* A = "data";


int main()
{
  char str1[8];
  char str2[8];

  strcpy(str1, A);
  scanf("%s" ,str2);

  printf("%s - str1\n", str1);
  printf("%s - str2\n", str2);

  return 0;
}

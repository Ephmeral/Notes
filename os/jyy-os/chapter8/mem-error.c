#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main() {
  int a[10], b = 0;
  printf("b = %d\n", b);
  for (int volatile i = 0; i <= 10; i++) {
    *(a + i) = 99;  // Writing to a[10] is UB
  }
  printf("b = %d\n", b);
}
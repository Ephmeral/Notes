#include <stdio.h>
#include <unistd.h>

int main() {
  write(1, "Hello World\n", 12);
  printf("%d + %d = %d\n", 1, 1, 1 + 1);
}
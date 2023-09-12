#include "thread.h"

int x = 0;

void Thello(int id) {
  x++;
  printf("%d\n", x);
}

int main() {
  for (int i = 0; i < 10; i++) {
    create(Thello);
  }
}
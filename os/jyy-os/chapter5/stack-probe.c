#include "thread.h"
#include <stdint.h>

void * volatile low[64];
void * volatile high[64];

void update_range(int T, void *ptr) {
    if (ptr < low[T]) low[T] = ptr;
    if (ptr > high[T]) high[T] = ptr;
}

void probe(int T, int n) {
  update_range(T, &n);
  long sz = (uintptr_t)high[T] - (uintptr_t)low[T];
  if (sz % 1024 < 32) {
    printf("Stack(T%d) >= %ld KB\n", T, sz / 1024);
  }
  probe(T, n + 1);  // Infinite recursion
}

void Tprobe(int T) {
  low[T] = (void *)-1;
  high[T] = (void *)0;
  update_range(T, &T);
  probe(T, 0);
}

int main() {
  setbuf(stdout, NULL);
  for (int i = 0; i < 4; i++) {
    create(Tprobe);
  }
}
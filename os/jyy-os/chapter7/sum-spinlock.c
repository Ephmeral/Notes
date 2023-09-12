#include "thread.h"

#define N 100000000
#define M 10

long sum = 0;

int xchg(int volatile *ptr, int newval) {
  int result;
  asm volatile(
    "lock xchgl %0, %1"
    : "+m"(*ptr), "=a"(result)
    : "1"(newval)
    : "memory"
  );
  return result;
}

int locked = 0;

void lock() {
  while (xchg(&locked, 1)) ;
}

void unlock() {
  xchg(&locked, 0);
}

void Tsum() {
  long nround = N / M;
  for (int i = 0; i < nround; i++) {
    lock();
    for (int j = 0; j < M; j++) {
      sum++;  // Non-atomic; can optimize
    }
    unlock();
  }
}

int main() {
  assert(N % M == 0);
  create(Tsum);
  create(Tsum);
  join();
  printf("sum = %ld\n", sum);
}
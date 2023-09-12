#include "os-real.h"

int count = 0;

void *Tprint(void *s) {
  char buf[64];
  for (int i = 0; i < 3; i++) {
    sprintf(buf, "#%02d Hello from %c%d\n", ++count, *(const char *)s, i);
    sys_write(buf);
    sys_sched();
  }
  return NULL;
}

int main() {
  int n = sys_choose(3) + 3;
  char buf[64];
  sprintf(buf, "#Thread = %d\n", n);
  sys_write(buf);
  for (int i = 0; i < n; i++) {
    sys_spawn(Tprint, &"ABCDE"[i]);
  }
}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int rdrand() {
  int ret;
  asm volatile("rdrand %0" : "=r"(ret));
  return ret;
}

int fd;

__attribute__((constructor)) 
void init() {
  fd = open("/dev/random", O_RDONLY);
  if (fd < 0) {
    perror("open random");
    abort();
  }
}

int rdrand_from_os() {
  int ret;
  read(fd, &ret, sizeof(ret));
  return ret;
}

int main() {
  int x = 0;

  x = rdrand();
  printf("x = %d\n", x);

  x = rdrand_from_os();
  printf("x = %d\n", x);
}
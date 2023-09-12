#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

static pthread_t threads[64];
static int nthreads = 0;

static inline void
sys_write(const char *s) {
  printf("%s", s);
  fflush(stdout);
}

static inline void
sys_sched() {
  usleep(rand() % 10000);
}

static inline void
sys_spawn(void *(*fn)(void *), void *args) {
    pthread_create(&threads[nthreads++], NULL, fn, args);
}

static inline int
sys_choose(int x) {
  return rand() % x;
}

// Constructor called before main()
static inline void __attribute__((constructor))
srand_init() {
  srand(time(0));
}

// Destructor called after main()
static inline void __attribute__((destructor))
thread_join() {
  for (int i = 0; i < nthreads; i++) {
    pthread_join(threads[i], NULL);  // Wait for thread terminations
  }
}
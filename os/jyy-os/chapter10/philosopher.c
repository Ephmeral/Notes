#include "thread.h"
#include "thread-sync.h"

#define N 5

sem_t table, avail[N];

void Tphilosopher(int id) {
  int lhs = (id + N - 1) % N;
  int rhs = id % N;
  while (1) {
    // Come to table
    P(&table);

    P(&avail[lhs]);
    printf("+ %d by T%d\n", lhs, id);
    P(&avail[rhs]);
    printf("+ %d by T%d\n", rhs, id);

    // Eat

    printf("- %d by T%d\n", lhs, id);
    printf("- %d by T%d\n", rhs, id);
    V(&avail[lhs]);
    V(&avail[rhs]);

    // Leave table
    V(&table);
  }
}

int main() {
  SEM_INIT(&table, N - 1);
  for (int i = 0; i < N; i++) {
    SEM_INIT(&avail[i], 1);
  }
  for (int i = 0; i < N; i++) {
    create(Tphilosopher);
  }
}
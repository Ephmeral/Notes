#include "thread.h"
#include "thread-sync.h"

sem_t fill, empty;

void Tproduce() {
  while (1) {
    P(&empty);
    printf("(");
    V(&fill);
  }
}

void Tconsume() {
  while (1) {
    P(&fill);
    printf(")");
    V(&empty);
  }
}

int main(int argc, char *argv[]) {
  assert(argc == 2);
  SEM_INIT(&fill, 0);
  SEM_INIT(&empty, atoi(argv[1]));
  for (int i = 0; i < 8; i++) {
    create(Tproduce);
    create(Tconsume);
  }
}
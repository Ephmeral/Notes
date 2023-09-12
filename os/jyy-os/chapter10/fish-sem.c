#include "thread.h"
#include "thread-sync.h"

#define LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

enum { A = 1, B, C, D, E, F, };

struct rule {
  int from, ch, to;
} rules[] = {
  { A, '<', B },
  { B, '>', C },
  { C, '<', D },
  { A, '>', E },
  { E, '<', F },
  { F, '>', D },
  { D, '_', A },
};
int current = A;
sem_t cont[128];

void fish_before(char ch) {
  P(&cont[(int)ch]);

  // Update state transition
  for (int i = 0; i < LENGTH(rules); i++) {
    struct rule *rule = &rules[i];
    if (rule->from == current && rule->ch == ch) {
      current = rule->to;
    }
  }
}

void fish_after(char ch) {
  int choices[16], n = 0;

  // Find enabled transitions
  for (int i = 0; i < LENGTH(rules); i++) {
    struct rule *rule = &rules[i];
    if (rule->from == current) {
      choices[n++] = rule->ch;
    }
  }

  // Activate a random one
  int c = rand() % n;
  V(&cont[choices[c]]);
}

const char roles[] = ".<<<<<>>>>___";

void fish_thread(int id) {
  char role = roles[id];
  while (1) {
    fish_before(role);
    putchar(role);  // Not lock-protected
    fish_after(role);
  }
}

int main() {
  setbuf(stdout, NULL);
  SEM_INIT(&cont['<'], 1);
  SEM_INIT(&cont['>'], 0);
  SEM_INIT(&cont['_'], 0);
  for (int i = 0; i < strlen(roles); i++)
    create(fish_thread);
}
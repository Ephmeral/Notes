#include <am.h>
#include <klib.h>
#include <klib-macros.h>

typedef union task {
  struct {
    const char *name;
    union task *next;
    void      (*entry)(void *);
    Context    *context;
  };
  uint8_t stack[8192];
} Task;

Task *current;

void func(void *arg) {
  while (1) {
    putch(*(char *)arg);
    for (int volatile i = 0; i < 100000; i++) ;
  }
}

Task tasks[] = {
  { .name = "a", .entry = func },
  { .name = "b", .entry = func },
};

Context *on_interrupt(Event ev, Context *ctx) {
  if (!current) {
    current = &tasks[0];
  } else {
    current->context = ctx;
  }
  return (current = current->next)->context;
}

int main() {
  cte_init(on_interrupt);  // call on_interrupt() on traps/interrupts

  for (int i = 0; i < LENGTH(tasks); i++) {
    Task *task    = &tasks[i];
    Area stack    = (Area) { &task->context + 1, task + 1 };
    task->context = kcontext(stack, task->entry, (void *)task->name);
    task->next    = &tasks[(i + 1) % LENGTH(tasks)];
  }
  iset(true);  // Enable external interrupts (timer, I/O, ...)
  yield();  // Trap
}
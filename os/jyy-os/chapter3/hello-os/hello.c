#include <am.h>

#define ESC "\033["
#define RED ESC "01;31m"
#define CLR ESC "0m"

const char *MESSAGE = RED "Hello, OS World\n" CLR;

int main() {
  for (const char *s = MESSAGE; *s; s++) {
    putch(*s);  // Prints to platform-dependent debug console
  }
}
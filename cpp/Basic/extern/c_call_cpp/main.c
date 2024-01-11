#include <stdio.h>

extern void print_msg(const char*msg);

int main() {
    print_msg("hello world!");
}
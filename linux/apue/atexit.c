#include <stdio.h>
#include <stdlib.h>

static void my_exit1(void);
static void my_exit2(void);
static void my_exit3(void);

int main() {
    if (atexit(my_exit1)) {
        fprintf(stderr, "cannot register my_exit1");
        exit(1);
    }
    if (atexit(my_exit2)) {
        fprintf(stderr, "cannot register my_exit2");
        exit(1);
    }
    if (atexit(my_exit3)) {
        fprintf(stderr, "cannot register my_exit3");
        exit(1);
    }
    printf("main is done\n");
}

void my_exit1() {
    printf("my exit1\n");
}

void my_exit2() {
    printf("my exit2\n");
}

void my_exit3() {
    printf("my exit3\n");
}
/*
$ ./a.out 
main is done
my exit3
my exit2
my exit1
*/
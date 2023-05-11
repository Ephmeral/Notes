#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int gval = 10;

int main() {
    pid_t pid;
    int lval = 20;
    gval++, lval += 5;
    pid = fork();
    if (pid == 0) {
        gval += 2;
        lval += 2;
        printf("Child process: [%d %d]\n", gval, lval);
    } else {
        gval -= 2;
        lval -= 2;
        printf("Parent process: [%d %d]\n", gval, lval);
    }
    return 0;
}
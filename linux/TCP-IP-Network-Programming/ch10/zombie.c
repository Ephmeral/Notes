#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        puts("Hi, I am a child process");
        puts("End child process");
    } else {
        printf("Child process id: %d\n", pid);
        sleep(30);
        puts("End parent process");
    }
    exit(0);
}
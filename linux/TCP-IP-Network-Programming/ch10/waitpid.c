#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int status;
    pid_t pid = fork();

    if (pid == 0) {
        sleep(15);
        return 24;
    } else {
        /* while (!wait(&status)) { */ // 使用wait会阻塞，而waitpid不会阻塞
        while (!waitpid(-1, &status, WNOHANG)) {
            sleep(3);
            puts("sleep 3 second");
        }
        if (WIFEXITED(status)) {
            printf("Child send %d \n", WEXITSTATUS(status));
        }
    }
    exit(0);
}
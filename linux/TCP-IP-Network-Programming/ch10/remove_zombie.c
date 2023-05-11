#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>       
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void read_childproc(int sig) {
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    if (WIFEXITED(status)) {
        printf("Remove proc pid is %d\n", pid);
        printf("Child send: %d\n", WEXITSTATUS(status));
    }
}

int main() {
    int status;

    struct sigaction act;
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, 0);

    pid_t pid = fork();

    if (pid == 0) { // 子进程
        puts("Hi, I am child process");
        sleep(10);
        return 12; 
    } else { // 父进程
        printf("Child process id: %d\n", pid);
        pid = fork();
        if (pid == 0) { // 另外一个子进程
            puts("Hi, I am other child process");
            sleep(10);
            exit(24);
        } else {
            printf("Child process id: %d\n", pid);
            for (int i = 0; i < 5; ++i) {
                puts("wait...");
                sleep(5);
            }
        }
    }
    exit(0);
}
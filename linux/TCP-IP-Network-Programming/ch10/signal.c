#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// 信号处理函数，处理SIGALRM信号
void timeout(int sig) {
    if (sig == SIGALRM) {
        puts("Time out!");
    }
    alarm(2); // 每隔2s产生SIGALRM信号
}

// 信号处理函数，处理SIGINT信号
void keycontrol(int sig) {
    if (sig == SIGINT) {
        puts("CTRL + C pressed");
    }
}

int main() {
    // 注册信号处理函数
    signal(SIGALRM, timeout); 
    signal(SIGINT, keycontrol);
    // 2s后产生SIGALRM信号
    alarm(2); 

    for (int i = 0; i < 3; ++i) {
        puts("wait...");
        sleep(100);
    }
    exit(0);
}
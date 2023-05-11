#define _XOPEN_SOURCE // 这行是避免在vscode中爆红
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
    struct sigaction act_time;
    act_time.sa_handler = timeout;
    sigemptyset(&act_time.sa_mask);
    act_time.sa_flags = 0;

    struct sigaction act_keycontrol;
    act_keycontrol.sa_handler = keycontrol;
    sigemptyset(&act_keycontrol.sa_mask);
    act_keycontrol.sa_flags = 0;

    sigaction(SIGALRM, &act_time, 0);
    sigaction(SIGINT, &act_keycontrol, 0);
    alarm(2);
    for (int i = 0; i < 3; ++i) {
        puts("wait...");
        sleep(100);
    }
    exit(0);
}
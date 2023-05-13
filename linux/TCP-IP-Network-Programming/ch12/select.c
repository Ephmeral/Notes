#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <time.h>
#define BUF_SIZE 30

int main() {
    fd_set reads, temps;
    FD_ZERO(&reads); // 初始化reads
    FD_SET(0, &reads);  // 监视文件描述符0，即标准输入流的变化

    struct timeval timeout;
    char buf[BUF_SIZE];
    
    while (1) {
        temps = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        int result = select(1, &temps, 0, 0, &timeout);
        if (result == -1) {
            puts("select() error!");
            break;
        } else if (result == 0) {
            puts("timeout!");
        } else {
            if (FD_ISSET(0, &temps)) {
                int len = read(0, buf, BUF_SIZE);
                buf[len] = 0;
                printf("Message from console: %s", buf);
            }
        }
    }
}
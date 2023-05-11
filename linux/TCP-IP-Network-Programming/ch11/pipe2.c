#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 30

int main() {
    int fd[2];
    char msg1[] = "Who are you?";
    char msg2[] = "Thank you for your message";
    char buf[BUF_SIZE];
    
    pipe(fd); // 创建管道
    int pid = fork();
    if (pid == 0) { 
        write(fd[1], msg1, sizeof(msg1)); // 写入msg1
        sleep(2); // 此行代码注释之后在运行，可能会发生阻塞，原因是先读的进程会把数据先取走
        read(fd[0], buf, BUF_SIZE);
        printf("Child process output: %s\n", buf);
    } else { 
        read(fd[0], buf, BUF_SIZE);
        printf("Parent process output: %s\n", buf);
        write(fd[1], msg2, sizeof(msg2));
        sleep(3);
    }
    exit(0);
}
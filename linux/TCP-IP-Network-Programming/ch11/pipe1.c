#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 30

int main() {
    int fd[2];
    char msg[] = "Who are you?";
    char buf[BUF_SIZE];
    
    pipe(fd); // 创建管道
    int pid = fork();
    if (pid == 0) { // 子进程写入消息到管道中
        // sleep(10); // 来检查管道默认是否是阻塞的
        write(fd[1], msg, sizeof(msg));
    } else { // 父进程从管道中读取数据
        printf("parent process\n");
        read(fd[0], buf, BUF_SIZE);
        puts(buf);
    }
    exit(0);
}
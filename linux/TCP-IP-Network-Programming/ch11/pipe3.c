#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 30

int main() {
    int fd1[2], fd2[2];
    char msg1[] = "Who are you?";
    char msg2[] = "Thank you for your message";
    char buf[BUF_SIZE];
    
    pipe(fd1); pipe(fd2); // 创建2个管道
    int pid = fork();
    if (pid == 0) { 
        write(fd1[1], msg1, sizeof(msg1)); // 写入msg1
        sleep(2); // 此行代码此时注释了也不会阻塞，因为用了两个管道
        read(fd2[0], buf, BUF_SIZE);
        printf("Child process output: %s\n", buf);
    } else { 
        read(fd1[0], buf, BUF_SIZE);
        printf("Parent process output: %s\n", buf);
        write(fd2[1], msg2, sizeof(msg2));
        sleep(3);
    }
    exit(0);
}
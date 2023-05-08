#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv) {
    char buf[] = "Let's go!\n";
    // O_CREAT | O_WRONLY | O_TRUNC 是文件打开模式，将创建新文件，并且只能写。如存在 data.txt 文件，则清空文件中的全部数据。
    int fd = open("data.txt", O_CREAT | O_WRONLY | O_TRUNC, 0600);
    if (fd == -1) {
        perror("open()");
        exit(1);
    }
    printf("file descriptor: %d \n", fd);
    // 向对应 fd 中保存的文件描述符的文件传输 buf 中保存的数据。
    if (write(fd, buf, sizeof(buf)) == -1) {
        perror("write()");
        exit(1);
    }
    close(fd);

    exit(0);
}
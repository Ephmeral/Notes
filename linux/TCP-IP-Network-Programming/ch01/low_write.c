#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#define BUF_SIZE 100

int main() {
    int fd;
    char buf[BUF_SIZE];

    fd = open("data.txt", O_RDONLY);
    if (fd == -1) {
        perror("open()");
        exit(1);
    }
    printf("file descriptor: %d \n", fd);

    if (read(fd, buf, sizeof(buf)) == -1){
        perror("read()");
        exit(1);
    }
    printf("file data: %s", buf);
    close(fd);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main() {
    struct stat buf;
    if (stat("/etc/passwd", &buf) == -1) {
        perror("/etc/passwd");
    } else {
        printf("The size of /etc/passwd is %ld\n", buf.st_size);
    }
    exit(0);
}
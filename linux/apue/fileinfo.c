#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
void show_stat_info(char *fname, struct stat *buf);

int main(int argc, char **argv) {
    struct stat buf;
    if (argc < 2) {
        fprintf(stderr, "Usage: fileinfo fname");
        exit(1);
    }
    if (stat(argv[1], &buf) == -1) {
        perror("stat()");
    } else {
        show_stat_info(argv[1], &buf);
    }
    exit(0);
}

void show_stat_info(char *fname, struct stat *buf) {
    printf("mode: %o\n", buf->st_mode);
    printf("links:%ld\n", buf->st_nlink);
    printf("user:%d\n", buf->st_uid);
    printf("group:%d\n", buf->st_gid);
    printf("size:%ld\n", buf->st_size);
    printf("mode time:%ld\n", buf->st_mtime);
    printf("name:%s\n", fname);
}
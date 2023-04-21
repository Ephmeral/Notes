#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

void do_ls(char dirname[]) {
    DIR *dir_ptr;
    struct dirent *direntp;
    if ((dir_ptr = opendir(dirname)) == NULL) {
        fprintf(stderr, "ls: cannot open %s", dirname);
    } else {
        while ((direntp = readdir(dir_ptr)) != NULL) {
            printf("%s\n", direntp->d_name);
        }
        closedir(dir_ptr);
    }
}

int main(int argc, char **argv) {
    if (argc == 1) {
        do_ls(".");
    } else {
        while (argc--) {
            printf("%s:\n", *++argv);
            do_ls(*argv);
        }
    }
    exit(0);
} 
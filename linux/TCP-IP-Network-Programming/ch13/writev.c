#include <stdio.h>
#include <sys/uio.h>

int main() {
    struct iovec vec[2];
    char buf1[] = "ABCDEFG";
    char buf2[] = "1234567890";
    
    vec[0].iov_base = buf1;
    vec[0].iov_len = 3;
    vec[1].iov_base = buf2;
    vec[1].iov_len = 4;

    int len = writev(1, vec, 2);
    puts("");
    printf("write bytes %d\n", len);
    return 0;
}
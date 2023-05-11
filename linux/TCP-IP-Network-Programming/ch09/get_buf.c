#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char **argv) {
    int sock, state;
    int send_buf, recv_buf;
    socklen_t optlen;

    optlen = sizeof(send_buf);
    sock = socket(PF_INET, SOCK_STREAM, 0);

    // 获取socket类型
    state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&send_buf, &optlen);
    if (state) {
        error_handling("getsockopt() error");
    }
    // 获取socket类型
    optlen = sizeof(recv_buf);
    state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&recv_buf, &optlen);
    if (state) {
        error_handling("getsockopt() error");
    }

    printf("Input buffer size: %d\n", recv_buf);
    printf("Output buffer size: %d\n", send_buf);
    exit(0);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <math.h>
#define BUF_SIZE 100

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    struct timeval timeout;
    fd_set reads, cpy_reads;
    socklen_t adr_sz;
    int fd_max, str_len, fd_num;
    char buf[BUF_SIZE];
    // 服务器端创建socket
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    
    // 绑定到任意的地址
    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) {
        error_handling("bind() error");
    }
    // 服务器监听socket
    if (listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }
    // 初始化select监听的参数，将服务端的socket注册到select上
    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads);
    fd_max = serv_sock;
    
    while (1) {
        cpy_reads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        if ((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1) {
            break;
        }
        if (fd_num == 0) continue;

        for (int i = 0; i < fd_max + 1; ++i) {
            // 检查是否有状态发生变化的文件描述符
            if (FD_ISSET(i, &cpy_reads)) {
                // 如果是服务端发生变化，则建立连接请求
                if (i == serv_sock) {
                    adr_sz = sizeof(clnt_addr);
                    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &adr_sz);
                    FD_SET(clnt_sock, &reads);
                    if (clnt_sock > fd_max) {
                        fd_max = clnt_sock;
                    }
                    printf("connected client: %d\n", clnt_sock);
                } else { // 否则则处理变化得到的数据
                    str_len = read(i, buf, BUF_SIZE);
                    if (str_len == 0) {
                        FD_CLR(i, &reads);
                        close(i);
                        printf("closed client: %d\n", i);
                    } else {
                        write(i, buf, str_len);
                    }
                }
            }
        }
    }
    close(serv_sock);

    exit(0);
}
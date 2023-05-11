#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUFSIZE 1024

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
    socklen_t clnt_addr_size;
    char message[BUFSIZ];

    // 创建socket，此时不区分server还是client，需要等后续的操作才会区分
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) {
        error_handling("socket() error");
    }
    int option, str_len;
    socklen_t optlen, clnt_adr_sz;
    optlen = sizeof(option);
    option = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void*)&option, optlen);

    // 初始化socket地址
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    // 通过bind函数绑定socket地址
    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }
    // 服务器监听socket
    if (listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }
    clnt_addr_size = sizeof(clnt_addr);
    for (int i = 0; i < 5; ++i) {
        // accept接受客户端的连接
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1) {
            error_handling("accept() error");
        } else {
            printf("Connected client %d\n", i);
        }
        // 从客户端读取数据，并直接写回
        int str_len = 0;
        while ((str_len = read(clnt_sock, message, BUFSIZE)) != 0) {
            write(clnt_sock, message, str_len);
        }
        close(clnt_sock);
    }
    close(serv_sock);
    exit(0);
}
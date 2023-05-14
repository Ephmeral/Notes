#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#define BUF_SIZE 30

int recv_sock, acpt_sock;

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
    struct sockaddr_in recv_adr, serv_adr;
    int len, state;
    socklen_t recv_adr_sz;
    char buf[BUF_SIZE];

    acpt_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&recv_adr, 0, sizeof(recv_adr));
    recv_adr.sin_family = AF_INET;
    recv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    recv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(acpt_sock, (struct sockaddr*)&recv_adr, sizeof(recv_adr))) {
        error_handling("bind() error");
    } 
    if (listen(acpt_sock, 5) == -1) {
        error_handling("listen() error");
    }

    recv_adr_sz = sizeof(serv_adr);
    recv_sock = accept(acpt_sock, (struct sockaddr*)&serv_adr, &recv_adr_sz);

    while (1) {
        len = recv(recv_sock, buf, sizeof(buf) - 1, MSG_PEEK | MSG_DONTWAIT);
        if (len > 0) break;
    }
    buf[len] = 0;
    printf("Buffering %d bytes: %s\n", len, buf);
    
    len = recv(recv_sock, buf, sizeof(buf) - 1, 0);
    buf[len] = 0;
    printf("Read again: %s\n", buf);

    close(recv_sock);
    close(acpt_sock);
    exit(0);
}
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

void urg_handle() {
    
    char buf[BUF_SIZE];
    int len = recv(recv_sock, buf, sizeof(buf) - 1, MSG_OOB);
    buf[len] = 0;
    printf("Urgent Message: %s\n", buf);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    struct sockaddr_in recv_adr, serv_adr;
    int len, state;
    socklen_t serv_adr_sz;
    struct sigaction act;
    char buf[BUF_SIZE];

    act.sa_handler = urg_handle;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

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

    serv_adr_sz = sizeof(serv_adr);
    recv_sock = accept(acpt_sock, (struct sockaddr*)&serv_adr, &serv_adr_sz);

    fcntl(recv_sock, __F_SETOWN, getpid());
    state = sigaction(SIGURG, &act, 0);

    while ((len = recv(recv_sock, buf, sizeof(buf), 0)) != 0) {
        if (len == -1) continue;
        buf[len] = 0;
        puts(buf);
    }
    close(recv_sock);
    close(acpt_sock);
    exit(0);
}
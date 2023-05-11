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
// 读子进程
void read_routine(int sock, char *buf) {
    while (1) {
        int str_len = read(sock, buf, BUFSIZE);
        if (str_len == 0) return;
        buf[str_len] = 0;
        printf("message from server: %s\n", buf);
    }
}

// 写子进程
void write_routine(int sock, char *buf) {
    while (1) {
        // fputs("Input message(Q to quit): ", stdout);
        fgets(buf, BUFSIZE, stdin);        
        if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n")) {
            shutdown(sock, SHUT_WR);
            return;
        }
        write(sock, buf, strlen(buf));
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }
    int sock;
    struct sockaddr_in serv_addr;
    char buf[BUFSIZE];

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        error_handling("socket() error");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));
    // 通过connect向服务器发起连接请求
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("connect() error");
    } else {
        puts("Connected......");
    }
    
    int pid = fork();
    if (pid == 0) {
        write_routine(sock, buf);
    } else {
        read_routine(sock, buf);
    }
    close(sock);
    exit(0);
}
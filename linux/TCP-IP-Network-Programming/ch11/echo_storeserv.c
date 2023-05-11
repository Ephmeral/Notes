#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#define BUF_SIZE 100

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

void read_childproc(int sig) {
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    if (WIFEXITED(status)) {
        printf("Remove proc pid is %d\n", pid);
        printf("Child send: %d\n", WEXITSTATUS(status));
    }
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

    struct sigaction act;
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, 0);

    // 创建socket，此时不区分server还是client，需要等后续的操作才会区分
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) {
        error_handling("socket() error");
    }
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

    int fd[2];
    pipe(fd);
    int pid = fork();
    if (pid == 0) {
        FILE *fp = fopen("echomsg.txt", "wt");
        char msgbuf[BUF_SIZE];
        for (int i = 0; i < 10; ++i) {
            int len = read(fd[0], msgbuf, BUF_SIZE);
            fwrite((void*)msgbuf, 1, len, fp);
        }
        fclose(fp);
        return 0;
    }

    // 多进程处理客户端的连接
    while (1) {
        clnt_addr_size = sizeof(clnt_addr);
        // accept接受客户端的连接
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1) {
            continue;
        } else {
            printf("new client connected\n");
        }
        // 创建一个子进程，响应客户端的连接请求
        int pid = fork();
        if (pid < 0) {
            close(clnt_sock);
            continue;
        } else if (pid == 0) {
            // 关闭不需要的文件描述符
            close(serv_sock); 
            // 从客户端读取数据，并直接写回
            int str_len = 0;
            while ((str_len = read(clnt_sock, message, BUF_SIZE)) != 0) {
                write(clnt_sock, message, str_len);
                write(fd[1], message, str_len);
            }
            close(clnt_sock);
            puts("client disconnected...");
            return 0;
        } else {
            close(clnt_sock);
        }
    }
    close(serv_sock);
    exit(0);
}
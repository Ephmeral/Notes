#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <math.h>
#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>
#define BUF_SIZE 4
#define EPOLL_SIZE 50

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

void setnonblockingmode(int fd) {
    int flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;

    socklen_t adr_sz;
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
    struct epoll_event event;
    int epfd = epoll_create(EPOLL_SIZE), event_cnt;
    struct epoll_event *ep_events = malloc(sizeof(struct epoll_event)*EPOLL_SIZE);
    event.events = EPOLLIN;
    event.data.fd = serv_sock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);
    
    while (1) {
        event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if (event_cnt == -1) {
            puts("epoll_wait() errot");
            break;
        }
        puts("return epoll_wait");
        for (int i = 0; i < event_cnt; ++i) {
            // 检查是否有状态发生变化的文件描述符
            if (ep_events[i].data.fd == serv_sock) {
                adr_sz = sizeof(clnt_addr);
                clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &adr_sz);
                setnonblockingmode(clnt_sock);
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = clnt_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
                printf("connected client: %d\n", clnt_sock);
            } else { // 否则则处理变化得到的数据
                while (1) {
                    int str_len = read(ep_events[i].data.fd, buf, BUF_SIZE);
                    if (str_len == 0) {
                        epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
                        close(ep_events[i].data.fd);
                        printf("closed client: %d\n", ep_events[i].data.fd);
                        break;
                    } else if (str_len < 0) {
                        if (errno == EAGAIN) {
                            break;
                        }
                    } else {
                        write(ep_events[i].data.fd, buf, str_len);
                    }
                }
                
            }
        }
    }
    close(serv_sock);
    exit(0);
}
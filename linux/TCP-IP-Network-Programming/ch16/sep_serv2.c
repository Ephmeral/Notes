#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(0);
    }

    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
    listen(serv_sock, 5);

    clnt_adr_sz = sizeof(clnt_adr);
    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

    FILE *readfp = fdopen(clnt_sock, "r");
    FILE *writefp = fdopen(dup(clnt_sock), "w");

    fputs("From server: Hi~ client?\n", writefp);
    fputs("I love all of the world\n", writefp);
    fputs("You are awesome\n", writefp);
    fflush(writefp);

    // 半关闭
    shutdown(fileno(writefp), SHUT_WR);
    fclose(writefp);
  
    char buf[30] = {0};  
    fgets(buf, sizeof(buf), readfp);
    fputs(buf, stdout);
    fclose(readfp);
    return 0;
}
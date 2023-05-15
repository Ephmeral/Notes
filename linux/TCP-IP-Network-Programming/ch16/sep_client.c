#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(0);
    }

    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    int sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr));

    FILE *readfp = fdopen(sock, "r");
    FILE *writefp = fdopen(sock, "w");

    char buf[30] = {0};
    while (1) {
        if (fgets(buf, sizeof(buf), readfp) == NULL) {
            break;
        }
        fputs(buf, stdout);
        fflush(stdout);
    }

    fputs("From Client: Thank you!\n", writefp);
    fflush(writefp);
    fclose(writefp);
    fclose(readfp);
    return 0;
}
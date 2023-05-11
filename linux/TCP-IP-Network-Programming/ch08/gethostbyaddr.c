#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <ip>\n", argv[0]);
        exit(1);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    struct hostent *host = gethostbyaddr((char*)&addr.sin_addr, 4, AF_INET);
    if (!host) {
        error_handling("gethost.....error");
    }
    
    printf("Official name: %s\n", host->h_name);
    for (int i = 0; host->h_aliases[i]; ++i) {
        printf("Aliases %d: %s\n", i + 1, host->h_aliases[i]);
    }
    printf("Address type: %s\n", (host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INEF6");
    for (int i = 0; host->h_addr_list[i]; ++i) {
        printf("IP addr %d: %s\n", i + 1, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
    }

    exit(0);
}
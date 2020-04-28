#include "unpv13e/lib/unp.h"

void str_cli(FILE* fp, int sockfd){
    char sendline[MAXLINE], recvline[MAXLINE];

    while (Fgets(sendline, MAXLINE, fp) != NULL){
        Writen(sockfd, sendline, strlen(sendline));

        if(Readline(sockfd, recvline, MAXLINE) == 0){
            err_quit("str_cli: server terminate prematurely");
        }
        Fputs(recvline, stdout);
    }
}

int main(int argc, char* argv[]) {
    int sockfd;
    struct sockaddr_in serveraddr;

    if(argc != 2){
        err_quit("usage: tcpclient <IPaddress>");
    }

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERV_PORT);
    Inet_pton(AF_INET, argv[1], &serveraddr.sin_addr);

    Connect(sockfd, (SA*) &serveraddr, sizeof(serveraddr));
    str_cli(stdin, sockfd);

    exit(0);
    return 0;
}

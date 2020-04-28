#include "unpv13e/lib/unp.h"

struct args{
    long arg1;
    long arg2;
};

struct result{
    long sum;
};

void str_cli(FILE* fp, int sockfd){
    char sendline[MAXLINE];
    struct args myArgs;
    struct result myResult;

    while (Fgets(sendline, MAXLINE, fp) != NULL){
        if(sscanf(sendline, "%ld%ld", &myArgs.arg1, &myArgs.arg2) != 2) {
            printf("invalid input: %s", sendline);
            continue;
        }

        Writen(sockfd, &myArgs, sizeof(struct args));
        printf("input from the client is %ld%ld\n", myArgs.arg1, myArgs.arg2);

        if(Readn(sockfd, &myResult, sizeof(myResult)) == 0){
            err_quit("str_cli: server terminate prematurely");
        }
        printf("Get result: %ld\n", myResult.sum);
    }
}


void str_cli_select(FILE* fp, int sockfd){
    int maxfdp1;
    fd_set rset;
    char sendline[MAXLINE], recvline[MAXLINE];

    FD_ZERO(&rset);

    while(1){
        FD_SET(fileno(fp), &rset);
        FD_SET(&sockfd, &rset);

        maxfdp1 = max(fileno(fp), &rset);
        Select(maxfdp1, &rset, NULL, NULL, NULL);

        if(FD_ISSET(sockfd, &rset)){
            if(Readline(sockfd, recvline, MAXLINE) == 0){
                err_quit("str_cli: server terminated prematurely");
            }
            Fputs(recvline, stdout);
        }

        if(FD_ISSET(fileno(fp), &rset)){
            if(Fgets(sendline, MAXLINE, fp) == NULL){
                return;
            }
            Writen(sockfd, sendline, strlen(sendline));
        }
    }
}

int main(int argc, char* argv[]) {
    int sockfd, i;
    struct sockaddr_in serveraddr;

    if(argc != 2){
        err_quit("usage: tcpclient <IPaddress>");
    }

    sockfd= Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERV_PORT);
    Inet_pton(AF_INET, argv[1], &serveraddr.sin_addr);

    Connect(sockfd, (SA*) &serveraddr, sizeof(serveraddr));

    str_cli(stdin, sockfd);

    exit(0);
    return 0;
}

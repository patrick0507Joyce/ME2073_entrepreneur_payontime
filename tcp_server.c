#include "unpv13e/lib/unp.h"
struct args{
    long arg1;
    long arg2;
};

struct result{
    long sum;
};

void str_echo(int sockfd){
    ssize_t n;
//    char buf[MAXLINE];
    struct args myArgs;
    struct result myResult;
    again:
    while(1){
        //connection is closed by client
        if((n = Readn(sockfd, &myArgs, sizeof(struct args))) == 0){
            return;
        }
        myResult.sum = myArgs.arg1 + myArgs.arg2;
        printf("the result is %ld from server\n", myResult.sum);

        Writen(sockfd, &myResult, sizeof(myResult));
    }
    if(n < 0 && errno == EINTR){
        goto again;
    }
    else if( n < 0){
        err_sys("Server side: read error");
    }
}
//signal handler for SIGCHLD
void sig_child(int signo){
    pid_t pid;
    int stat;

    while ((pid = waitpid(-1, &stat, WNOHANG) > 0)){
        printf("child %d terminated\n", pid);
    }
    return;
}


int main(int argc, char* argv[]){
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clientlen;
    struct sockaddr_in cliaddr, servaddr;
    void sig_child(int);

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Bind(listenfd, (SA*) &servaddr, sizeof(servaddr));
    Listen(listenfd, LISTENQ);
    Signal(SIGCHLD, sig_child);

    while(1){
        clientlen = sizeof(cliaddr);
        if((connfd = accept(listenfd, (SA *)&cliaddr, &clientlen)) < 0){
            if(errno == EINTR){
                continue;
            }
            else{
                err_sys("accept error!");
            }
        }
        if((childpid = Fork()) == 0){
            Close(listenfd);
            str_echo(connfd);
            Close(connfd);
            exit(0);
        }
        Close(connfd);
    }

}
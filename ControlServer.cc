
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <poll.h>
#include <stdlib.h>

const char* UNIXDOMAIN_PATH = "/usr/local/movieplayer/server.sock";


int main(int argc, char *argv[]){

    int sock, clifd;
    struct sockaddr_un srvaddr, cliaddr;
    struct pollfd fds[1] = {0,};
    char buf[2048];

    /* ソケットの作成 */
    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if(sock < 0){
        fprintf(stderr, "socket error errno[%d]\n", errno);
        exit(-1);
    }

    remove(UNIXDOMAIN_PATH);

    memset(&srvaddr, 0, sizeof(struct sockaddr_un));
    srvaddr.sun_family = AF_UNIX;
    strcpy(srvaddr.sun_path, UNIXDOMAIN_PATH);

    if(bind(sock, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr_un)) < 0){
       fprintf(stderr, "bind error errno[%d]\n", errno);
       exit(-1);
    }

    if(listen(sock, 5) < 0){
        fprintf(stderr, "listen error errno[%d]\n", errno);
        exit(-1);
    }

    fds[0].fd = sock;
    fds[0].events = POLLIN;

    while(1){
        poll(fds, 1, -1);
        if (fds[0].revents & POLLIN) {
            /* sock1からデータを受信して表示します */
            memset(&cliaddr, 0, sizeof(struct sockaddr_un));
            memset(buf, 0, sizeof(buf));
            socklen_t addrlen = sizeof(struct sockaddr_un);
            if((clifd = accept(sock, (struct sockaddr *)&cliaddr, &addrlen)) < 0){
                fprintf(stderr, "accept error errno[%d]\n", errno);
                exit(-1);
            }
            int len = read(clifd, buf, sizeof(buf));
            buf[len] = 0;
            std::cout <<  buf << std::endl;
        }
    }

    close(sock);

    std::cout <<  "Server end." << std::endl;
    return 0;
}


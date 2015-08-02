
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

const char* UNIXDOMAIN_PATH = "/usr/local/movieplayer/server.sock";


int main(int argc, char *argv[]){
    int sock;
    struct sockaddr_un addr;

    if(argc != 2){
        fprintf(stderr, "require data.", errno);
        exit(-1);
    }


    /* ソケットの作成 */
    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if(sock < 0){
        fprintf(stderr, "socket error errno[%d]\n", errno);
        exit(-1);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, UNIXDOMAIN_PATH);

    if(connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) < 0){
       fprintf(stderr, "connect error errno[%d]\n", errno);
       exit(-1);
    }

    const char* data = argv[1];
    if(write(sock, data, strlen(data)) < 0){
        fprintf(stderr, "write error errno[%d]\n", errno);
        exit(-1);
    }
    
    close(sock);

    return 0;
}


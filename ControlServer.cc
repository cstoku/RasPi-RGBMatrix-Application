
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <poll.h>
#include <stdlib.h>
#include <thread>
#include <time.h>
#include <sys/time.h>


const char* UNIXDOMAIN_PATH = "/usr/local/movieplayer/server.sock";

int server_main(void);
void exit_program(int signum);

int endflag = 0;

int main(void){
    if (signal(SIGINT, exit_program) == SIG_ERR) {
        fprintf(stderr, "signal handler entry error.");
        exit(-1);
    }
    std::cout<<"handler entry."<<std::endl;
    std::thread th(server_main);
    std::cout<<"thread run."<<std::endl;
    struct timespec wait;
    wait.tv_sec=0;
    wait.tv_nsec=100000000L;
    std::cout<<"main loop.\nendflag:"<< !endflag <<std::endl;
    while(!endflag){
        std::cout << "." << std::flush;
        nanosleep(&wait, NULL);
    }
    std::cout<<"join thread."<<std::endl;
    th.join();

    return 0;
}


void exit_program(int signum){
    std::cout<<"SIGINT!!"<<std::endl;
    endflag=1;
}



int server_main(void){

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

    while(!endflag){
        poll(fds, 1, 100);
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
            std::cout <<  "\n" << buf << std::endl;
        }
    }

    close(sock);

    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <sys/epoll.h>

void client_thread(void *arg);

int main() {
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    
    struct sockaddr_in serveraddr;
    memset(&serveraddr,0,sizeof(struct sockaddr_in));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(2048);

    if(-1 == bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(struct sockaddr_in))){
        perror("bind error\n");
        return -1;
    }

    if(-1 == listen(sockfd,10)){
        perror("listen error\n");
        return -1;
    }
#if 0
    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);
    //accept is blocking function
    int clientfd = accept(sockfd,(struct sockaddr*)&clientaddr,&len);
   
    if(clientfd == -1){
        perror("accept error\n");
        return -1;
    }
    char buffer[128] = {0};
    //recv is blocking function too
    int count = recv(clientfd,buffer,128,0);
    if(count == -1){
        perror("reading msg error\n");
        return -1;
    }else if(count == 0){

    }
    send(clientfd,buffer,count,0);

    printf("sockefd : %d, clientfd : %d, buffer: %s\n",sockfd,clientfd,buffer);

    getchar();
    // it should disconnect by client, otherwise server will happen TIME_WAIT 
    close(clientfd)

#elif 0 
    while(1){
        struct sockaddr_in clientaddr;
        socklen_t len = sizeof(clientaddr);
        int clientfd = accept(sockfd,(struct sockaddr*)&clientaddr,&len);
        if(clientfd == -1){
            perror("accept error\n");
            return -1;
        }
        pthread_t thread;
        pthread_create(&thread,NULL,client_thread,&clientfd);
    }
#elif 0 //select
    
    // fd_set is a long type array. sizeof(long) = 4byte
    // select is using bit to listen event 
    // fd_set is long fds_ bits[32] 32(element's size) * 32(element's count) = 1024
    // so it can listen 1024 io normally
    fd_set rfds, rset;
    FD_ZERO(&rfds);
    FD_SET(sockfd, &rfds);
    
    // start from listen fd
    int maxfd = sockfd;

    while(1){
        rset = rfds;

        int nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        
        // check new connection, if have newconnect , sockfd will be perform readable by rset
        if(FD_ISSET(sockfd, &rset)){
            struct sockaddr_in clientaddr;
            socklen_t len = sizeof(clientaddr);
            int clientfd = accept(sockfd,(struct sockaddr*)&clientfd,&len);
        
            printf("accept clientfd : %d\n",clientfd);

            FD_SET(clientfd, &rfds);
            maxfd = clientfd;
        }

        // check clientfd is readable or not
        int i = 0;
        for(i = sockfd+1; i <= maxfd; i++){
            if(FD_ISSET(i,&rset)){
                char buffer[128] = {0};
                int count = recv(i,buffer,128,0);
                if(count == 0){
                    printf("closing connection\n");
                    FD_CLR(i,&rfds);
                    close(i);
                    break;
                }
                send(i,buffer,count,0);
                printf("clientfd : %d, count : %d, message : %s\n",i,count,buffer);
            }
        }
    }
#elif 0 // poll

    struct pollfd fds[1024] = {0};

    fds[sockfd].fd = sockfd;
    fds[sockfd].events = POLLIN;

    int maxfd = sockfd;

    while(1) {
        int nready = poll(fds, maxfd + 1, -1);
        if (fds[sockfd].revents & POLLIN) {
            struct sockaddr_in clientaddr;
            socklen_t len = sizeof(clientaddr);
            int clientfd = accept(sockfd,(struct sockaddr*)&clientaddr,&len);
        
            printf("accept clientfd : %d\n",clientfd);
            fds[clientfd].fd = clientfd;
            fds[clientfd].events = POLLIN;

            maxfd = clientfd;
        }
        int i = 0;
        for(i = sockfd + 1; i <= maxfd;i++) {
            if(fds[i].revents & POLLIN) {
                char buffer[128] = { 0 };
                int count = recv(i,buffer,128,0);
                if(count == 0){
                    printf("closing connection\n");
                    fds[i].fd = -1;
                    fds[i].events = 0;
                    close(i);
                    continue;
                }
                send(i,buffer,count,0);
                printf("clientfd : %d, count : %d, message : %s\n",i,count,buffer);
            }
        }
    }

#else // epoll
    int epfd = epoll_create(1);

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;

    epoll_ctl(epfd, EPOLL_CTL_ADD,sockfd,&ev);

    struct epoll_event events[1024] = { 0 };
    while(1) {
        int nready = epoll_wait(epfd,events,1024,-1);

        int i = 0;
        for(i = 0; i < nready; i++) {
            int connfd = events[i].data.fd;
            if(sockfd == connfd) {
                struct sockaddr_in clientaddr;
                socklen_t len = sizeof(clientaddr);
                int clientfd = accept(sockfd,(struct sockaddr*)&clientaddr,&len);

                ev.events = EPOLLIN;
                ev.data.fd = clientfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);
                printf("clientfd : %d \n",clientfd);
            }else if(events[i].events & EPOLLIN) {
                char buffer[128] = { 0 };
                int count = recv(connfd, buffer, 128, 0);
                if(count == 0){
                    printf("closing connection\n");
                    epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, NULL);
                    close(connfd);
                    continue;
                }
                send(connfd, buffer, count, 0);
                printf("clientfd : %d, count : %d, message : %s\n",connfd,count,buffer);
            }
        }
    }

#endif
    
    //return 0;
}

void client_thread(void* arg){
    int clientfd = *(int*)arg;

    while(1){
        char buffer[128] = {0};
        int count = recv(clientfd,buffer,128,0);
        if(count == 0){
            printf("clientfd : %d is closing...\n",clientfd);
            close(clientfd);
            return;
        }
        printf("clientfd : %d receiving message count : %d, message : %s\n",clientfd,count,buffer);
        send(clientfd,buffer,count,0);
    }
    close(clientfd);
}
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

#define BUFFER_LENGTH 1024
typedef int (*RCALLBACK)(int fd);

int epfd;

struct conn_item{
    int fd;
    // char buffer[BUFFER_LENGTH];
    // int idx;
    
    char rbuffer[BUFFER_LENGTH];
    int rlength;

    char wbuffer[BUFFER_LENGTH];
    int wlength;

    // out of io 
    union 
    {
        RCALLBACK accept_call;
        RCALLBACK recv_call;
    }recv_t;


    RCALLBACK send_call;
};

struct conn_item connlist[1024] = { 0 };

void client_thread(void *arg);
int _accept_callback(int fd);
int _recv_callback(int fd);
int _send_callback(int fd);
int set_event(int fd, int event, int flag);

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

    connlist[sockfd].fd = sockfd;
    connlist[sockfd].recv_t.accept_call = _accept_callback;
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
    epfd = epoll_create(1);

    // struct epoll_event ev;
    // ev.events = EPOLLIN;
    // ev.data.fd = sockfd;
    //epoll_ctl(epfd, EPOLL_CTL_ADD,sockfd,&ev);
    set_event(sockfd,EPOLLIN,1);

    struct epoll_event events[1024] = { 0 };
    while(1) {
        int nready = epoll_wait(epfd,events,1024,-1);

        int i = 0;
        for(i = 0; i < nready; i++) {
            int connfd = events[i].data.fd;
            if(sockfd == connfd) {
#if 1
                //int clientfd = _accept_callback(connfd);
                int clientfd = connlist[sockfd].recv_t.accept_call(connfd);
#else
                struct sockaddr_in clientaddr;
                socklen_t len = sizeof(clientaddr);
                int clientfd = accept(sockfd,(struct sockaddr*)&clientaddr,&len);

                ev.events = EPOLLIN;
                ev.data.fd = clientfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);
#endif
                printf("clientfd : %d \n",clientfd);
            }else if(events[i].events & EPOLLIN) {
#if 1
                //int count = _recv_callback(connfd);
                int count = connlist[connfd].recv_t.recv_call(connfd);
                char* buffer = connlist[connfd].rbuffer;
#else
                char buffer[128] = { 0 };
                int count = recv(connfd, buffer, 128, 0);
                if(count == 0){
                    printf("closing connection\n");
                    epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, NULL);
                    close(connfd);
                    continue;
                }
                send(connfd, buffer, count, 0);
#endif
                count == 0 ? printf("\n") : printf("--> recv : %s\n",buffer);
            }else if(events[i].events & EPOLLOUT) {
                //int count = _send_callback(connfd);
                char* buffer = connlist[connfd].wbuffer;
                printf("--> send : %s\n",buffer);
                int count = connlist[connfd].send_call(connfd);
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

int _accept_callback(int fd) {
    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);
    int clientfd = accept(fd,(struct sockaddr*)&clientaddr,&len);

    // struct epoll_event ev;
    // ev.events = EPOLLIN;
    // ev.data.fd = clientfd;
    // epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);
    if(-1 == set_event(clientfd, EPOLLIN, 1)){
        perror("accept call back error\n");
        return -1;
    }

    connlist[clientfd].fd = clientfd;
    memset(connlist[clientfd].rbuffer,0,BUFFER_LENGTH);
    connlist[clientfd].rlength = 0;
    memset(connlist[clientfd].wbuffer,0,BUFFER_LENGTH);
    connlist[clientfd].wlength = 0;
    connlist[clientfd].recv_t.recv_call = _recv_callback;
    connlist[clientfd].send_call = _send_callback;

    return clientfd;
}

int _recv_callback(int fd) {
    char *buffer = connlist[fd].rbuffer;
    int count = recv(fd, buffer+connlist[fd].rlength, BUFFER_LENGTH, 0);
    if(count == 0){
        printf("closing connection\n");
        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
        close(fd);
        return -1;
    }
    connlist[fd].rlength += count;

#if 1 // echo 
    memcpy(connlist[fd].wbuffer,connlist[fd].rbuffer,connlist[fd].rlength);
    connlist[fd].wlength = connlist[fd].rlength;
#endif

    if(-1 == set_event(fd, EPOLLOUT, 0)){
        perror("recv call back error\n");
        return -1;
    }

    return count;
}

int _send_callback(int fd) {
    char *buffer = connlist[fd].wbuffer;
    int count = send(fd, buffer+connlist[fd].wlength, BUFFER_LENGTH, 0);
    if(count == -1){
        return -1;
    }

    if(-1 == set_event(fd, EPOLLIN, 0)){
        perror("send call back error\n");
        return -1;
    }
    
    return count;
}

int set_event(int fd, int event, int flag) {
    struct epoll_event ev;
    int isError;
    if(flag) {
        ev.events = event;
        ev.data.fd = fd;
        isError = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
        if(isError == -1){
            goto error;
        }
        return isError;
    }else {
        ev.events = event;
        ev.data.fd = fd;
        isError = epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
        if( isError == -1){
            goto error;
        }
        return isError;
    }
error:
    return isError;
}
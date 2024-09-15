#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

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

#else  
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
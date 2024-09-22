#ifndef __HANDLE_H
#define __HANDLE_H
#include "handle.h"

int SendCallback(int fd,char* buffer,int len){
    int count = 0;
    if((count = send(fd,buffer,len,0)) == -1){
        cerr << "< Receive error >" << endl;
        close(fd);  
    }
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    if(epoll_ctl(epfd, EPOLL_CTL_MOD,fd,&ev) == -1){
        cerr<<"< Epoll modify to EPOLLOUT error >"<<endl;
        close(fd);
        //return EXIT_FAILURE;
    }
    return count;
}

int RecvCallback(int fd,char* buffer,int len){
    int count = 0;
    if((count = recv(fd,buffer,len,0)) == 0){
        cout<<"[ Closing connection ]"<<endl;
        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
        close(fd);
    }else if(count == -1){
        cerr << "< Receive error >" << endl;
        close(fd);  
    }
    struct epoll_event ev;
    ev.events = EPOLLOUT;
    ev.data.fd = fd;
    if(epoll_ctl(epfd, EPOLL_CTL_MOD,fd,&ev) == -1){
        cerr<<"< Epoll modify to EPOLLOUT error >"<<endl;
        close(fd);
        //return EXIT_FAILURE;
    }
    return count;
}



#endif

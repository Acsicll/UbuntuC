#include <iostream>
#include "const.h"

char send_buffer[BUFFER_LENGTH];
unsigned int send_buffer_length = 0;
char recv_buffer[BUFFER_LENGTH];
unsigned int recv_buffer_length = 0; 

//ConcreateBuffer baseBuffers[1024] = { 0 };

void ResetSendAndRecvBuffer(){
    memset(send_buffer, 0, BUFFER_LENGTH);
    send_buffer_length = 0;
    memset(recv_buffer, 0, BUFFER_LENGTH);
    recv_buffer_length = 0;
}

int initServer(const string& ip_addr_str,unsigned short port,int maxbacklog){
    //using IPV4 - IPV6 TCP connection
    int server_sockfd;
    struct sockaddr_in server_address;
    struct in_addr ip_addr;

    memset(&server_address, 0, sizeof(server_address));
    if(ip_addr_str != ""){
        if(inet_pton(AF_INET, ip_addr_str.c_str(), &ip_addr) != 1){
            cerr<< "< Invalid IP address >" << endl;
            return EXIT_FAILURE;
        }
    }else{
        ip_addr.s_addr = INADDR_ANY;
    }

    server_sockfd = socket(AF_INET,SOCK_STREAM,0);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = ip_addr.s_addr;
    server_address.sin_port = htons(port);

    if(bind(server_sockfd,(struct sockaddr*)&server_address,sizeof(server_address)) == -1){
        cerr<< "< Bind socket error >" <<endl;
        close(server_sockfd);
        return EXIT_FAILURE;
    }

    if(listen(server_sockfd,maxbacklog) == -1){
        cerr<< "< Listen socket error >" <<endl;
        close(server_sockfd);
        return EXIT_FAILURE;
    }

    cout<<"[ Server is listening on port : "<<port<<" ]"<<endl;
    return server_sockfd;
}

int main(){
    string ip_addr_str = "";
    unsigned short port = 9000;
    int maxbacklog = 10;

    epfd = epoll_create(1);
    //for(int i = 0;i<maxbacklog;i++){
        int serverfd = initServer(ip_addr_str,port,maxbacklog);

        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = serverfd;

        if(epoll_ctl(epfd, EPOLL_CTL_ADD, serverfd, &ev) == -1){
            cerr << "< Epoll add error >" << endl;
            close(serverfd);
            return EXIT_FAILURE;
        }   
        ResetSendAndRecvBuffer();

        struct epoll_event events[1024] = { 0 };
        while(1){
            int nready = epoll_wait(epfd, events, 1024, -1);
            for(int i = 0;i<nready;i++){
                int readyfd = events[i].data.fd;
                if(serverfd == readyfd){
                    struct sockaddr_in client_address;
                    int clientfd;
                    socklen_t socklen = sizeof(client_address);
                    if((clientfd = accept(serverfd,(struct sockaddr*)&client_address,&socklen)) == -1){
                        cerr<<"< Accept socket error >"<<endl;
                        close(clientfd);
                        //return EXIT_FAILURE;
                        continue;
                    }
                    struct epoll_event ev;
                    ev.events = EPOLLIN;
                    ev.data.fd = clientfd;
                    if(epoll_ctl(epfd, EPOLL_CTL_ADD,clientfd,&ev) == -1){
                        cerr<<"< Epoll add new connection error >"<<endl;
                        close(clientfd);
                        //return EXIT_FAILURE;
                        continue;
                    }
                    cout<<"[ Clientfd : "<<clientfd<<" ]"<<endl;
                }else if(events[i].events & EPOLLIN){
                    int count = 0;
                    char *buffer = recv_buffer;
                    if((count = recv(readyfd,buffer,BUFFER_LENGTH,0)) == 0 ){
                        cout<<"[ Closing connection ]"<<endl;
                        epoll_ctl(epfd, EPOLL_CTL_DEL, readyfd, NULL);
                        close(readyfd);
                        //return EXIT_SUCCESS;
                        continue;
                    }else if(count == -1){
                        cerr << "< Receive error >" << endl;
                        close(readyfd);
                        continue;
                    }
                    recv_buffer_length = count;
                    struct epoll_event ev;
                    ev.events = EPOLLOUT;
                    ev.data.fd = readyfd;
                    if(epoll_ctl(epfd, EPOLL_CTL_MOD,readyfd,&ev) == -1){
                        cerr<<"< Epoll modify to EPOLLOUT error >"<<endl;
                        close(readyfd);
                        //return EXIT_FAILURE;
                        continue;
                    }
                    cout<<"[ Recv message : "<< recv_buffer<<" | count : "<<count<<" ]"<<endl;  
                }else if(events[i].events & EPOLLOUT){
                    int count = 0;
                    char *buffer = send_buffer;
                    memcpy(send_buffer,recv_buffer,recv_buffer_length);
                    cout<<"[ Send message : "<< send_buffer <<" | count : "<<recv_buffer_length<<" ]"<<endl;
                    if((count = send(readyfd, buffer,recv_buffer_length,0) )== -1){
                        cerr<<"< Send error >"<<endl;
                    }
                    send_buffer_length = count;
                    struct epoll_event ev;
                    ev.events = EPOLLIN;
                    ev.data.fd = readyfd;
                    if(epoll_ctl(epfd, EPOLL_CTL_MOD,readyfd,&ev) == -1){
                        cerr<<"< Epoll modify to EPOLLOUT error >"<<endl;
                        close(readyfd);
                        //return EXIT_FAILURE;
                        continue;
                    }
                }

            }
        }
        close(serverfd);
    //}
    return EXIT_SUCCESS;
}

#include<iostream>
#include<stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include<pthread.h>
#include"server.h"
#include"networkFunctions.h"
#include"networkStructures.h"
using namespace std;

#define BACKLOG 10

    template<class T, int(T::*mem_fn)()>
    void* thunk(void* p){
        int *ret_val=(int *)calloc(1,sizeof(int));
        *ret_val=((static_cast<T*>(p)->*mem_fn)());
        return ret_val;
    }

    void* Server::get_in_addr(struct sockaddr *sock){
        if(sock->sa_family==AF_INET){
            return &(((struct sockaddr_in *)sock)->sin_addr);
        }
        else if(sock->sa_family==AF_INET6){
            return &(((struct sockaddr_in6 *)sock)->sin6_addr);
        }
    }

    int Server::hosting(){
        struct addrinfo hints,*servinfo ,*p;
        struct sockaddr_storage client_info;
        this->sockets=(struct socketsOpened*)calloc(1,sizeof(struct socketsOpened));
        int retval=0;
        p=NULL;
        int yes=1;
        memset(&hints,0,sizeof hints);
        hints.ai_family=AF_UNSPEC;
        hints.ai_socktype=SOCK_STREAM;
        hints.ai_flags=AI_PASSIVE;
        getaddrinfo(NULL,this->portno,&hints,&servinfo);
        sock=-1;
        for(p=servinfo;p!=NULL;p=p->ai_next){
            if((sock=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1)
                continue;
            if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
                      cout<<"setsockopt\n";
                      close(sock);
                      continue;
                      retval=SETSOCKOPT_ERROR;
            }
            if(bind(sock,p->ai_addr,p->ai_addrlen)==-1){
                close(sock);
                continue;
            }
            break;
        }
        if(p==NULL){
            cout<<"Failed to Bind.\n";
            close(sock);
            retval=BIND_ERROR;
            return BIND_ERROR;
        }
        char hostname[50];
        gethostname(hostname,50);
        cout<<"Listening on port : "<<this->portno<<"\n";
        freeaddrinfo(servinfo);
        if(listen(sock,BACKLOG)==-1){
            cout<<"Listening Error.\n";
            close(sock);
            retval=LISTEN_ERROR;
            return LISTEN_ERROR;
        }

           socklen_t client_info_size = sizeof client_info;
            cout<<"Waiting for Connections....\n";
            client_sock=accept(sock,(struct sockaddr *)&client_info,&client_info_size);
            if(client_sock==-1||!accept_condition){
                retval=ACCEPT_ERROR;
                return ACCEPT_ERROR;
            }
            
            char clientname[50];
            inet_ntop(client_info.ss_family,get_in_addr((struct sockaddr *)&client_info),clientname,sizeof clientname);
            cout<<"Connected to : "<<clientname<<"\n";
            cout<<"Connected at port : "<<this->portno<<"\n";
            struct ::socketsOpened *abc;
            Server::sockets->server_socket=sock;
            Server::sockets->client_socket=client_sock;
            Server::sockets->function=this->function;
            Server::sockets->additionalData=this->additionalData;
            if(pthread_create(&recvThread,NULL,::recvData,sockets)!=0)
                cout<<"Server cannot receive.\n";
   
            if(onConnection!=NULL)
                this->onConnection(this->conn_args);
            return SERVER_SUCCESS;
    }
    Server::Server(){
        accept_condition=true;
    }


    int Server::start(char *portno,void*(connected)(void *),void *conn_args,void *(receivedDataProcessor)(void *),void *args){
        cout<<"Port is : "<<portno<<"\n";
        this->portno=portno;
        this->function=receivedDataProcessor;
        this->additionalData=args;
        this->onConnection=connected;
        this->conn_args=conn_args;
        pthread_create(&hostThread,NULL,thunk<Server,&Server::hosting>,this);
    }

    int Server::start_unthreaded(char *portno,void*(connected)(void *),void *conn_args,void *(receivedDataProcessor)(void *),void *args){
        this->portno=portno;
        this->function=receivedDataProcessor;
        this->additionalData=args;
        this->onConnection=connected;
        this->conn_args=conn_args;
        return hosting();
    }

    pthread_t *Server::getThreadID(){
        return &(this->hostThread);
    }

int Server::sendData(void *n,int size){
    string s=(char *)n;
    s+="~";
    int numbytes=send(client_sock,(void *)(&s[0]),size+1,0);
    if(errno==EPIPE||numbytes==-1){
        close(sock);
        close(client_sock);
        return SEND_ERROR;
    }
    return SEND_SUCCESS;
}
void Server::shut(void){
	cout<<"Server shutting down....\n";
    close(sock);
    close(client_sock);
}

#include "MyServer.h"
#include  <bits/stdc++.h>
#include <arpa/inet.h>          
#include <signal.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include<stdio.h>
#include<string.h>
#include <netdb.h>
#include "parse.h"
#include "senddata.h"
#include "thread_pool.h"

#define MAXCONNECTION 10

using namespace std ;



void dispatch_to_here(void *arg){
    struct clientIdentity *cid_t = (struct clientIdentity*)arg ; 
    struct clientIdentity cid ;  
    cid.acceptId = cid_t->acceptId ; 
    cid.ip = cid_t->ip ; 
    cid.portno = cid_t->portno ; 
    cid.requesttime  = cid_t->requesttime ; 
    // cout << pthread_self()<<endl;
    Parse *P = new Parse();
    P->parseRequest(cid) ; 

}



u_int16_t get_port_number(struct sockaddr *s)
{
    return (((struct sockaddr_in  *)s)->sin_port);
}

//Return Ip address
void * get_ip_address(sockaddr *s)
{
    return &((sockaddr_in *)s)->sin_addr;
}



int getConnection(int port_int)
{
    cout << "process id is " <<  getpid() << endl;
    int sockId =0;
    string port = to_string(port_int) ; 
    
    struct addrinfo inValue, *serverInfo, *validInfo;
    struct sockaddr_storage clientAddr;
    int acceptId, address,yes;
    char ip[INET6_ADDRSTRLEN];
    char ip1[INET6_ADDRSTRLEN];
    socklen_t addrlen;


    memset(&inValue, 0, sizeof(inValue));
    inValue.ai_family = AF_INET;            // works for both ipv4 ipv6
    inValue.ai_socktype = SOCK_STREAM;      // tcp connection
    inValue.ai_flags = AI_PASSIVE;          //this is used
       //by applications that intend to communicate with peers running on the
       //same host.
    yes=1;

    
    //struct sockaddr_in serverInfo;
    if (getaddrinfo(NULL, port.c_str(), &inValue, &serverInfo) != 0)
            perror("Get Address:");

    for(validInfo = serverInfo; validInfo != NULL; validInfo = validInfo->ai_next) {
        if((sockId = (socket(validInfo->ai_family, validInfo->ai_socktype,0))) == -1)
                perror("Socket:");
        
        addrlen = sizeof(serverInfo);
        
        if (setsockopt(sockId, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1){
            perror("setsockopt");
            break;
        }
        
        if(bind(sockId,validInfo->ai_addr, validInfo->ai_addrlen) == -1)
                perror("Bind:");
        
        break;
    }

    // successfully done with bind
    
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)validInfo->ai_addr;
    void *addr;
    addr = &(ipv4->sin_addr);
    inet_ntop(validInfo->ai_family,addr, ip, sizeof(ip));
    freeaddrinfo(serverInfo);
    if(listen(sockId, MAXCONNECTION) == -1)
        perror("Listen:");


//==============================================

    threadpool tp;
    tp = create_threadpool(7);
    printf("ThreadPool created with 7 threads\n");



//=============================================


    while(true) {       // Main thread will listen continously 
        address = sizeof(clientAddr);
        // cout << "great" << endl;
        if((acceptId = accept(sockId,(struct sockaddr*)&clientAddr,(socklen_t *)&address)) == -1)
                perror("Accept:");
        cout << "accept time " << clock() << endl;

        inet_ntop(clientAddr.ss_family,get_ip_address((struct sockaddr *)&clientAddr),ip1, sizeof(ip1));
        u_int16_t clientport = get_port_number((struct sockaddr *)&(validInfo));
        time_t tim=time(NULL);
        tm *now=gmtime(&tim);
        char currtime[50];
        
        if (strftime(currtime, 50,"%x:%X", now) == 0)
                perror("Date Error");
        
        string requesttime(currtime);
        clientIdentity cid;
        cid.acceptId = acceptId;
        string s(ip1);
        cid.ip=s;
        cid.portno = clientport;
        cid.requesttime = requesttime;
        // P->parseRequest(cid);
        // cout << "great to go"<<endl; 
        dispatch(tp, dispatch_to_here, (void *) &cid);     

    }
}




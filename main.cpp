#include <bits/stdc++.h>
#include <unistd.h>
#include <errno.h>
#include "thread_pool.h"
#include "MyServer.h"
#include <arpa/inet.h>          /* inet_ntoa */
#include <dirent.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
using namespace std;



void dispatch_to_me()
{
   
    int default_port = 8080,listenfd;
    // char buf[256];
    // char *path = getcwd(buf, 256);
    
    

    listenfd = getConnection(default_port);
    if (listenfd > 0) {
        printf("listen on port %d, fd is %d\n", default_port, listenfd);
    }
     else {
        perror("ERROR");
        exit(listenfd);
    }

}


int main(){

    // threadpool tp;
     // tp = create_threadpool(7);
     // printf("ThreadPool created with 7 threads\n");
     int i=1;  
     dispatch_to_me() ; 
     // dispatch(tp, dispatch_to_me, (void *) i);     
      
    fprintf(stdout, "**main** done first\n");
    sleep(1);
    fprintf(stdout, "\n\n");
     
    fprintf(stdout, "**main done second\n");
     

      // destroy_threadpool(tp);
      sleep(5);
      exit(-1);
    
    return 0;
}
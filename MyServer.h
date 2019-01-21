#include<bits/stdc++.h>
using namespace std;

#ifndef INC_MY_SERVER_H
#define INC_MY_SERVER_H


// #define LISTENQ  1024  /* second argument to listen() */
// #define MAXLINE 1024   /* max length of a line */
// #define RIO_BUFSIZE 1024


// typedef struct {
//     int rio_fd;                 /* descriptor for this buf */
//     int rio_cnt;                /* unread byte in this buf */
//     char *rio_bufptr;           /* next unread byte in this buf */
//     char rio_buf[RIO_BUFSIZE];  /* internal buffer */
// } rio_t;

//  Simplifies calls to bind(), connect(), and accept() 
// typedef struct sockaddr SA;


// typedef struct {
//     char filename[512];
//     off_t offset;              /* for support Range */
//     size_t end;
// } http_request;

// typedef struct {
//     const char *extension;
//     const char *mime_type;
// } mime_map;
 



// //**************
// void rio_readinitb(rio_t *rp, int fd);
// ssize_t writen(int fd, void *usrbuf, size_t n);
// static ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n);
// ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);
// void format_size(char* buf, struct stat *stat);
// void handle_directory_request(int out_fd, int dir_fd, char *filename);
// static const char* get_mime_type(char *filename);

// void url_decode(char* src, char* dest, int max);

// void parse_request(int fd, http_request *req);
// void parse_request(int fd, http_request *req);
// void log_access(int status, struct sockaddr_in *c_addr, http_request *req);
// void client_error(int fd, int status, char *msg, char *longmsg);
// void serve_static(int out_fd, int in_fd, http_request *req,size_t total_size);
// void process(int fd, struct sockaddr_in *clientaddr);

//******************


int getConnection(int port);


#endif
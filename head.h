#ifndef __HEAD_H
#define __HEAD_H
#include "log.h"
#include <stdio.h>                                                     
#include <limits.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <time.h>
#define MAXFDS 1024
#define BUFFER_SIZE 4096
#define PIDNUM 2
#define SRVPORT 2948
int setnonblocking (int fd);
void addfd( int epollfd, int fd, int enable_et, int noblock);
void delfd(int epollfd, int fd);
int Process(int sockfd, int epfd);
int CreSrvNetSock(); 
int CreateWorkProc(int pipefd[][2], int num);  
int LoadLevel(int pidnum);
void send_fd(int fd, int fd_to_send); 
int WorkProc(int readfd); 
int CreateLogProc(int pipefd[2],int* buffmax);

#endif


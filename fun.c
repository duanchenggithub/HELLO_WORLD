#include "head.h"
int setnonblocking (int fd)
{
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option|O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return old_option;
}
void addfd( int epollfd, int fd, int enable_et, int noblock) 
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN;
	if( enable_et )
	{
		event.events |= EPOLLET;
	}
	if( noblock )	
	{
		setnonblocking(fd);
	}
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
}
void delfd(int epollfd, int fd)
{
	struct epoll_event event;
	event.data.fd = fd;
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &event);
}

int Process(int sockfd, int epfd)
{
	char buf[BUFFER_SIZE]={0}; //需要多次接收才能接收完成的一个消息的时候，buf该怎么保存，多个socket
	int ret =0;
    int sum =0;
	while(1)
	{
		memset(buf, 0, BUFFER_SIZE);
		ret=recv(sockfd, buf, BUFFER_SIZE-1, 0);
		if(ret < 0)
		{
			if( (errno == EAGAIN) || (errno == EWOULDBLOCK) )
				break;   //end recv
		}
		else if( ret == 0)  
		{
			delfd(epfd, sockfd);
			close(sockfd);
            return 0;
		}
		else 
		{
            sum += ret;
			printf("recv %s ,work process,sum=%d\n",buf,sum);
		}
	
	}
	return sum;
}
int CreSrvNetSock()
{
    int listenfd=0;
    int on = 1;
    struct sockaddr_in srvaddr;

    if(-1 == (listenfd=socket(AF_INET,SOCK_STREAM,0)))
    {
        printf("create net socket error\n");
        exit(0); 
    }
    memset(&srvaddr,0,sizeof(srvaddr));
    srvaddr.sin_family=AF_INET;
    srvaddr.sin_port=htons(SRVPORT);
    srvaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)); 
if(-1 == bind(listenfd,(struct sockaddr*)&srvaddr,sizeof(srvaddr)))
    {
        printf("bind net socket error \n");
        exit(0);
    }
    if(-1 == listen(listenfd,5))
    {   
        printf("listen net socket error \n");
        exit(0);
    }
    return listenfd;
}

int CreateWorkProc(int pipefd[][2], int num)
{
    int i = 0;
    int pid = 0;
    int ret = 0;
    Errlog( __FILE__, __LINE__, "CreateWorkProc");//test

    for(i=0; i<num; i++)
    {
        ret = socketpair(PF_UNIX, SOCK_DGRAM, 0, pipefd[i]);
        pid = fork();
        if(pid == 0)
        {
            close(pipefd[i][1]);
            WorkProc(pipefd[i][0]);
            exit(0);
        }
        else if(pid > 0)
        {
            close(pipefd[i][0]);
        }
    }
    return 0;
}

static const int CONTROL_LEN = CMSG_LEN(sizeof(int));
void send_fd(int fd, int fd_to_send)
{
    struct iovec iov[1];
    struct msghdr msg;
    char buf[0];

    iov[0].iov_base = buf;
    iov[0].iov_len = 1;

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    struct cmsghdr cm;
    cm.cmsg_len = CONTROL_LEN;
    cm.cmsg_level = SOL_SOCKET;
    cm.cmsg_type = SCM_RIGHTS;

    *(int  *)CMSG_DATA( &cm ) = fd_to_send;

    msg.msg_control = &cm;
    msg.msg_controllen = CONTROL_LEN;

    sendmsg(fd, &msg, 0);
}

int recv_fd(int fd)
{
    struct iovec iov[1];
    struct msghdr msg;
    char buf[0];

    iov[0].iov_base = buf;
    iov[0].iov_len = 1;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    struct  cmsghdr cm;
    msg.msg_control = &cm;
    msg.msg_controllen = CONTROL_LEN;

    recvmsg(fd, &msg, 0);

    int fd_to_read = *(int *)CMSG_DATA(&cm);
    return fd_to_read;
}    

int LoadLevel(int pidnum)
{
       srand((unsigned)time(NULL)); 
       return rand() % pidnum; 
} 

int WorkProc(int readfd)
{
    int workepfd = 0;
    int nfd = 0;
    int i=0;
    int sockfd =0;
    int connfd =0; 
    struct epoll_event workevs[MAXFDS];

    workepfd = epoll_create(MAXFDS);
    addfd(workepfd, readfd, 0, 0);
    while(1)
    {
        nfd=epoll_wait(workepfd, workevs, MAXFDS, -1);
        for(i=0; i<nfd; ++i)
        {
            sockfd = workevs[i].data.fd;
            if(sockfd == readfd)
            {
                connfd=recv_fd(sockfd);
                addfd(workepfd, connfd, 1, 1); 
            } 
            else 
            {
                Process(sockfd,workepfd);
            }
        }
    }
    return 0;
}


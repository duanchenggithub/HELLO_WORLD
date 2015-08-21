#include "head.h"
int main(int argc,char* argv[])
{
	int netlisfd,nfd,workfd;
    int i = 0;
    int sockfd = 0;
	int epfd = 0;
	size_t lenaddr = 0;
    int level = 0;
    int pipefd[PIDNUM][2]={{0}};
	struct sockaddr_in cliaddr;
	struct epoll_event events[MAXFDS];

    netlisfd = CreSrvNetSock();
    CreateWorkProc(pipefd, PIDNUM);

	epfd=epoll_create(MAXFDS); 
    addfd(epfd, netlisfd, 0, 0);

	while(1)
	{
		fflush(stdout);
        
		nfd=epoll_wait(epfd, events, MAXFDS, -1);
		for(i=0;i<nfd;++i)
		{
            sockfd = events[i].data.fd;
			if(sockfd == netlisfd)
			{
				memset(&cliaddr,0,sizeof(cliaddr));
				lenaddr=sizeof(cliaddr);
				workfd=accept(netlisfd,(struct sockaddr*)&cliaddr,&lenaddr);
			    if(workfd != -1)	
				{
                    level = LoadLevel(PIDNUM);//意思意思
                    SendFd(pipefd[level][1],workfd);
                    //???close();
				}
			}
		}//for
	}	//while
	close(epfd);
	return 0;
}

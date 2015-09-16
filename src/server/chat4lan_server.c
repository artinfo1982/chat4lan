#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(int argc,char *argv[])
{
	if (argc < 3)
	{
		printf("Usage ./xxx ip port\n");
		exit(1);
	}
	
	int fd = -1;
	switch (fork())
	{
		case -1:
			printf("fork failed\n");
			exit(1);
		case 0:
			break;
		default:
			exit(0);
	}
	if (-1 == setsid())
	{
		printf("setsid failed\n");
		exit(1);
	}	
	umask(0); 	
	if ((fd = open("/dev/null", O_RDWR)) == -1) 
	{
		printf("open /dev/null failed\n");
		exit(1);
	}
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		printf("dup2 STDIN_FILENO failed\n");
		exit(1);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		printf("dup2 STDOUT_FILENO failed\n");
		exit(1);
	}
	if (dup2(fd, STDERR_FILENO) == -1)
	{
		printf("dup2 STDERR_FILENO failed\n");
		exit(1);
	}
	close(fd);

	int sfd, cfd, efd, flag, rep, nfds, i;
	char buffer[8192];
    	struct epoll_event sev,cev, events[256];
	struct sockaddr_in servAddr;
	struct sockaddr_in cliAddr;
	
    	memset(&servAddr, 0, sizeof(servAddr));
	memset(&cliAddr, 0, sizeof(cliAddr));
	socklen_t addrlen = sizeof(cliAddr);
    	servAddr.sin_family = AF_INET;
    	servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    	servAddr.sin_port = htons((unsigned short)atoi(argv[2]));

	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket create failed\n");
		exit(1);
	}
	if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &rep, sizeof(rep)) < 0)
	{
		printf("socket set reuse addr failed\n");
		close(sfd);
		exit(1);
	}
	if (bind(sfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
	{
		printf("socket bind failed\n");
		close(sfd);
		exit(1);
	}
	if (listen(sfd, 64) < 0)
	{
		printf("socket listen failed\n");
		close(sfd);
		exit(1);
	}
	if ((efd = epoll_create(2048)) < 0)
	{
		printf("create epoll failed\n");
		close(sfd);
		exit(1);
	}
	sev.data.fd = sfd;  
    	sev.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &sev))
	{
		printf("register socket to epoll failed\n");
		close(sfd);
		close(efd);
		exit(1);
	}
	for (;;)
	{
		nfds = epoll_wait(efd, events, 256, -1);
		if (nfds < 0)
		{
			printf("epoll wait error\n");
			close(sfd);
			close(efd);
			exit(1);
		}
		for (i=0; i<nfds; i++)
		{
			if(events[i].data.fd == sfd)
			{
				if ((cfd = accept(sfd,(struct sockaddr *)&cliAddr, &addrlen)) < 0)
				{
					printf("acept failed\n");
					continue;
				}
				memset(buffer, 0x0, sizeof(buffer));
				if (recv(cfd, buffer, 8192, 0) < 0)
				{
					printf("recv failed\n");
					close(cfd);
					continue;
				}
				if (0x01 == buffer[0])
				{
					send(cfd, "AAA", 3, 0);
				}
			}
			close(cfd);
		}
	}
	return 0;
}

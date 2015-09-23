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
#include <sys/time.h>
#include <time.h>

#define  SERVER_IP  "127.0.0.1"
#define  CHEN_PORT  60001
#define  QIANG_PORT  60002
#define  CHENG_PORT  60003
#define  ZHANG_PORT  60004
#define  CHEN_FLAG  0x01
#define  QIANG_FLAG  0x02
#define  CHENG_FLAG  0x03
#define  ZHANG_FLAG  0x04

static int name_flag;
static char timer[20];

void get_time()
{
	struct timeval tv;
	struct tm tim;
	gettimeofday(&tv, NULL);
	localtime_r(&(tv.tv_sec), &tim);
	sprintf(timer, "%02d-%02d-%02d %02d:%02d:%02d", 
		tim.tm_year+1900,
		tim.tm_mon+1,
		tim.tm_mday,
		tim.tm_hour,
		tim.tm_min,
		tim.tm_sec);
}

void * server (void * arg)
{
	int sfd, cfd, efd, flag, rep, nfds, i;
	char buffer[8192];
	unsigned short port;
	switch (name_flag)
	{
		case 1 :
			port = (unsigned short)CHEN_PORT; break;
		case 2 :
			port = (unsigned short)QIANG_PORT; break;
		case 3 :
			port = (unsigned short)CHENG_PORT; break;
		case 4 :
			port = (unsigned short)ZHANG_PORT; break;
		default :
			printf("server thread can not get self name\n"); exit(1);
	}
    	struct epoll_event sev,cev, events[256];
	struct sockaddr_in servAddr;
	struct sockaddr_in cliAddr;
    	memset(&servAddr, 0, sizeof(servAddr));
	memset(&cliAddr, 0, sizeof(cliAddr));
	socklen_t addrlen = sizeof(cliAddr);
    	servAddr.sin_family = AF_INET;
    	servAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    	servAddr.sin_port = htons(port);

	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket create failed\n");
		exit(1);
	}
	if (fcntl(sfd, F_GETFL, 0) < 0)
	{
		printf("sfd socket set non-blocking step one F_GETFL failed\n");
		close(sfd);
		exit(1);
	}
	if (fcntl(sfd, F_SETFL, flag|O_NONBLOCK) < 0)
	{
		printf("sfd socket set non-blocking step one F_SETFL failed\n");
		close(sfd);
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
				get_time();
				if (buffer[0] == 0x1)
				{
					printf("陈栋 %s\n", timer);
					printf("%s\n", buffer);
				}					
				else if (buffer[0] == 0x2)
				{
					printf("强清清 %s\n", timer);
					printf("%s\n", buffer);
				}
				else if (buffer[0] == 0x3)
				{
					printf("程林林 %s\n", timer);
					printf("%s\n", buffer);
				}
				else if (buffer[0] == 0x4)
				{
					printf("张晓红 %s\n", timer);
					printf("%s\n", buffer);
				}
				else
					printf("illegal message\n");
			}
			close(cfd);
		}
	}
}

void * client (void * arg)
{
	int sfd1 = -1, sfd2 = -1, sfd3 = -1, ret1 = -1, ret2 = -1, ret3 = -1, size;
	char s_data[8192];
	struct sockaddr_in addr1, addr2, addr3;
	memset(&addr1, 0x0, sizeof(addr1));
	memset(&addr2, 0x0, sizeof(addr2));
	memset(&addr3, 0x0, sizeof(addr3));
	addr1.sin_family = AF_INET;	
	addr1.sin_addr.s_addr = inet_addr(SERVER_IP);
	addr2.sin_family = AF_INET;	
	addr2.sin_addr.s_addr = inet_addr(SERVER_IP);
	addr3.sin_family = AF_INET;	
	addr3.sin_addr.s_addr = inet_addr(SERVER_IP);
	switch (name_flag)
	{
		case 1 :
			addr1.sin_port = htons((unsigned short)QIANG_PORT);
			addr2.sin_port = htons((unsigned short)CHENG_PORT);
			addr3.sin_port = htons((unsigned short)ZHANG_PORT);
			break;
		case 2 :
			addr1.sin_port = htons((unsigned short)CHEN_PORT);
			addr2.sin_port = htons((unsigned short)CHENG_PORT);
			addr3.sin_port = htons((unsigned short)ZHANG_PORT);
			break;
		case 3 :
			addr1.sin_port = htons((unsigned short)CHEN_PORT);
			addr2.sin_port = htons((unsigned short)QIANG_PORT);
			addr3.sin_port = htons((unsigned short)ZHANG_PORT);
			break;
		case 4 :
			addr1.sin_port = htons((unsigned short)CHEN_PORT);
			addr2.sin_port = htons((unsigned short)QIANG_PORT);
			addr3.sin_port = htons((unsigned short)CHENG_PORT);
			break;
		default :
			printf("client thread can not get self name\n"); exit(1);
	}

	for (;;)
	{
		memset(s_data, 0x0, sizeof(s_data));
		scanf("%s", s_data + 1);
		switch (name_flag)
		{
			case 1:
				s_data[0] = (char)CHEN_FLAG; break;
			case 2:
				s_data[0] = (char)QIANG_FLAG; break;
			case 3:
				s_data[0] = (char)CHENG_FLAG; break;
			case 4:
				s_data[0] = (char)ZHANG_FLAG; break;
			default: break;
		}
		size = strlen(s_data + 1) + 2;
		sfd1 = socket(AF_INET, SOCK_STREAM, 0);
		sfd2 = socket(AF_INET, SOCK_STREAM, 0);
		sfd3 = socket(AF_INET, SOCK_STREAM, 0);
		ret1 = connect(sfd1, (struct sockaddr *)(&addr1), sizeof(struct sockaddr));
		ret2 = connect(sfd2, (struct sockaddr *)(&addr2), sizeof(struct sockaddr));
		ret3 = connect(sfd3, (struct sockaddr *)(&addr3), sizeof(struct sockaddr));
		if ((ret1 < 0) && (ret2 < 0) && (ret3 < 0))
		{
			close(sfd1);
			close(sfd2);
			close(sfd3);
			continue;
		}
		if (ret1 >= 0)
			send(sfd1, s_data, size, 0);
		if (ret2 >= 0)
			send(sfd2, s_data, size, 0);
		if (ret3 >= 0)
			send(sfd3, s_data, size, 0);
		close(sfd1);
		close(sfd2);
		close(sfd3);
	}
}

int main(int argc,char *argv[])
{
	if (strcmp(argv[0], "./chen") == 0)
		name_flag = 1;
	else if (strcmp(argv[0], "./qiang") == 0)
		name_flag = 2;
	else if (strcmp(argv[0], "./cheng") == 0)
		name_flag = 3;
	else if (strcmp(argv[0], "./zhang") == 0)
		name_flag = 4;
	else
	{
		printf("program name can only be chen or qiang or cheng or zhang\n");
		exit(1);
	}
	pthread_t t_server, t_client;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (pthread_create(&t_server, NULL, server,NULL) < 0)
	{
		perror("create t_server thread");
		exit(1);
	}
	if (pthread_create(&t_client, NULL, client,NULL) < 0)
	{
		perror("create t_server thread");
		exit(1);
	}
	pthread_join(t_server, NULL);
	pthread_join(t_client, NULL);
	return 0;
}

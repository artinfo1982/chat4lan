#include "Common.h"
#include "DbMysql.h"
#include "Log.h"
#include "Constants.h"

void 
setDaemon 
(FILE * fp)
{
	int fd = -1;
	struct timeval tv;
	struct tm tim;
	switch (fork())
	{
		case -1:
			ERROR_LOG(fp, tv, tim, "fork failed, errInfo : %s\n", strerror(errno));
		case 0:
			break;
		default:
			exit(0);
	}
	if (-1 == setsid())
		ERROR_LOG(fp, tv, tim, "fork setsid failed, errInfo : %s\n", strerror(errno));
	umask(0); 	
	if ((fd = open("/dev/null", O_RDWR)) == -1)
		ERROR_LOG(fp, tv, tim, "open /dev/null failed, errInfo : %s\n", strerror(errno));
	if (dup2(fd, STDIN_FILENO) == -1)
		ERROR_LOG(fp, tv, tim, "dup2 stdin_fileno error, errInfo : %s\n", strerror(errno));
	if (dup2(fd, STDOUT_FILENO) == -1)
		ERROR_LOG(fp, tv, tim, "dup2 stdout_fileno error, errInfo : %s\n", strerror(errno));
	if (dup2(fd, STDERR_FILENO) == -1)
		ERROR_LOG(fp, tv, tim, "dup2 stderr_fileno error, errInfo : %s\n", strerror(errno));
	close(fd);
	INFO_LOG(fp, tv, tim, "create daemon process successfully, daemon process pid=[%d], ppid=[1]\n", getpid());
}

void 
createMainProcess 
(FILE * fp, mainServiceType mst, char * ip, char * port)
{
	struct timeval tv;
	struct tm tim;
	switch (fork())
	{
		case -1:
			ERROR_LOG(fp, tv, tim, "fork main service process failed, errInfo : %s\n", strerror(errno));
		case 0:
			INFO_LOG(fp, tv, tim, "create main service process successfully, main service process pid=[%d], ppid=[%d]\n", getpid(), getppid());
			mst(fp, ip, port);
		default:
			break;
	}
}

void 
createHeartbeatProcess 
(FILE * fp, heartbeatServiceType hst, char * ip)
{
	struct timeval tv;
	struct tm tim;
	switch (fork())
	{
		case -1:
			ERROR_LOG(fp, tv, tim, "fork heartbeat service process failed, errInfo : %s\n", strerror(errno));
		case 0:
			INFO_LOG(fp, tv, tim, "create heartbeat service process successfully, heartbeat service process pid=[%d], ppid=[%d]\n", getpid(), getppid());
			hst(fp, ip);
		default:
			break;
	}
}

void 
initServerAddr 
(struct sockaddr_in* servAddr, char * serverIP, char * serverPort)
{
	memset(servAddr, 0, sizeof(struct sockaddr_in));
    	servAddr -> sin_family = AF_INET;
    	servAddr -> sin_addr.s_addr = inet_addr(serverIP);
    	servAddr -> sin_port = htons((unsigned short)atoi(serverPort));
}

void 
initClientAddr 
(struct sockaddr_in* cliAddr)
{
	memset(cliAddr, 0, sizeof(struct sockaddr_in));
}

int 
initListenSock 
(FILE * fp, struct sockaddr_in* servAddr)
{
	int sfd, rep;
	struct timeval tv;
	struct tm tim;
	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		ERROR_LOG(fp, tv, tim, "socket create failed, errInfo : %s\n", strerror(errno));
		exit(1);
	}		
	if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &rep, sizeof(int)) < 0)
	{
		close(sfd);
		ERROR_LOG(fp, tv, tim, "socket set reuse addr failed, errInfo : %s\n", strerror(errno));
		exit(1);
	}
	if (bind(sfd, (struct sockaddr*) servAddr, sizeof(struct sockaddr_in)) < 0)
	{
		close(sfd);
		ERROR_LOG(fp, tv, tim, "socket bind failed, errInfo : %s\n", strerror(errno));
		exit(1);
	}
	if (listen(sfd, 64) < 0)
	{
		close(sfd);
		ERROR_LOG(fp, tv, tim, "socket listen failed, errInfo : %s\n", strerror(errno));
		exit(1);
	}
	return sfd;
}

int 
initEpoll 
(FILE * fp, int sfd, struct epoll_event* sev)
{
	int efd;
	struct timeval tv;
	struct tm tim;
	if ((efd = epoll_create(2048)) < 0)
	{
		close(sfd);
		ERROR_LOG(fp, tv, tim, "create epoll failed, errInfo : %s\n", strerror(errno));
		exit(1);
	}
	sev -> data.fd = sfd;  
    	sev -> events = EPOLLIN | EPOLLET;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, sfd, sev))
	{
		close(sfd);
		close(efd);
		ERROR_LOG(fp, tv, tim, "register socket to epoll failed, errInfo : %s\n", strerror(errno));
		exit(1);
	}
	return efd;
}

int 
epollWait 
(FILE * fp, int efd, int sfd, struct epoll_event * events)
{
	int nfds;
	struct timeval tv;
	struct tm tim;
	if ((nfds = epoll_wait(efd, events, 256, -1)) < 0)
	{
		ERROR_LOG(fp, tv, tim, "epoll wait error, errInfo : %s\n", strerror(errno));
		close(sfd);
		close(efd);
		exit(1);
	}
	return nfds;
}

void 
sendFlagMsg 
(int sock, char flag)
{
	char * index = & flag;
	send(sock, index, 1, 0);
	close(sock);
}

int 
sendRecvHeartbeat 
(int sock)
{
	char data = (char)HEART_BEAT_REQ_RSP;
	char buffer[2];
	if (send(sock, &data, 1, 0) < 1)
	{
		close(sock);
		return 1;
	}
	if (recv(sock, buffer, 1, 0) < 1)
	{
		close(sock);
		return 1;
	}
	if (HEART_BEAT_REQ_RSP != buffer[0])
	{
		close(sock);
		return 1;
	}
	close(sock);
	return 0;
}

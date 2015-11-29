#include "Common.h"
#include "DbMysql.h"
#include "Service.h"
#include "Log.h"

FILE * fp_log;

int 
main 
(int argc,char *argv[])
{
	fp_log = fopen(LOG_PATH, "a+");
	
	if (argc < 3)
	{
		printf("Usage ./xxx ip port\n");
		exit(1);
	}

	//setDaemon(fp_log);

	int sfd, cfd, efd, flag, nfds, rep, i, j, res;
	char data[1024], buffer[1024];
    	struct epoll_event sev,cev, events[256];
	struct sockaddr_in servAddr;
	struct sockaddr_in cliAddr;
	struct timeval tv;
	struct tm tim;
	socklen_t addrlen = sizeof(struct sockaddr_in);

	initServerAddr(&servAddr, argv[1], argv[2]);
	initClientAddr(&cliAddr);
	sfd = initListenSock(fp_log, &servAddr);
	efd = initEpoll(fp_log, sfd, &sev);

	for (;;)
	{
		nfds = epollWait(fp_log, efd, sfd, events);
		for (i=0; i<nfds; i++)
		{
			if(events[i].data.fd == sfd)
			{
				if ((cfd = accept(sfd,(struct sockaddr *)&cliAddr, &addrlen)) < 0)
				{
					ERROR_LOG(fp_log, tv, tim, "accept failed, errInfo : %s\n", strerror(errno));
					continue;
				}
				if (recv(cfd, buffer, 1024, 0) < 0)
				{
					ERROR_LOG(fp_log, tv, tim, "recv failed, errInfo : %s\n", strerror(errno));
					close(cfd);
					continue;
				}
				switch (buffer[0])
				{
					case 0x01:
					{
						loginHandler(fp_log,argv[1],MYSQL_USRNAME,MYSQL_USRPASS,MYSQL_DBNAME,MYSQL_DBPORT,cfd,buffer,data);
						break;
					}
					case 0x02:
					{
						registerHandler(fp_log,argv[1],MYSQL_USRNAME,MYSQL_USRPASS,MYSQL_DBNAME,MYSQL_DBPORT,cfd,buffer,data);
						break;
					}
					case 0x03:
					{
						addFriendHandler(fp_log,argv[1],MYSQL_USRNAME,MYSQL_USRPASS,MYSQL_DBNAME,MYSQL_DBPORT,cfd,buffer,data);
						break;
					}
					case 0x04:
					{
						p2pChatHandler(fp_log,argv[1],MYSQL_USRNAME,MYSQL_USRPASS,MYSQL_DBNAME,MYSQL_DBPORT,cfd,buffer,data);
						break;
					}
					case 0x05:
					{
						addGroupHandler(fp_log,argv[1],MYSQL_USRNAME,MYSQL_USRPASS,MYSQL_DBNAME,MYSQL_DBPORT,cfd,buffer,data);
						break;
					}
					case 0x06:
					{
						groupChatHandler(fp_log,argv[1],MYSQL_USRNAME,MYSQL_USRPASS,MYSQL_DBNAME,MYSQL_DBPORT,cfd,buffer,data);
						break;
					}
					default:
						break;
				}
			}
		}
	}
	return 0;
}

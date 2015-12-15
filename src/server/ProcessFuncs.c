#include "Common.h"
#include "DbMysql.h"
#include "Service.h"
#include "Log.h"
#include "ProcessFuncs.h"

void 
mainService (FILE * fp, char * ip, char * port)
{
	int sfd, cfd, efd, nfds, i;
	char data[1024], buffer[1024];
    	struct epoll_event sev, events[256];
	struct sockaddr_in servAddr;
	struct sockaddr_in cliAddr;
	struct timeval tv;
	struct tm tim;
	socklen_t addrlen = sizeof(struct sockaddr_in);

	initServerAddr(&servAddr, ip, port);
	initClientAddr(&cliAddr);
	sfd = initListenSock(fp, &servAddr);
	efd = initEpoll(fp, sfd, &sev);

	for (;;)
	{
		nfds = epollWait(fp, efd, sfd, events);
		for (i=0; i<nfds; i++)
		{
			if(events[i].data.fd == sfd)
			{
				if ((cfd = accept(sfd,(struct sockaddr *)&cliAddr, &addrlen)) < 0)
				{
					ERROR_LOG(fp, tv, tim, "accept failed, errInfo : %s\n", strerror(errno));
					continue;
				}
				if (recv(cfd, buffer, 1024, 0) < 0)
				{
					ERROR_LOG(fp, tv, tim, "recv failed, errInfo : %s\n", strerror(errno));
					close(cfd);
					continue;
				}
				switch (buffer[0])
				{
					case 0x01:
					{
						loginHandler(fp,ip,MYSQL_USRNAME,MYSQL_USRPASS,MYSQL_DBNAME,MYSQL_DBPORT,cfd,buffer,data);
						break;
					}
					case 0x02:
					{
						registerHandler(fp,ip,MYSQL_USRNAME,MYSQL_USRPASS,MYSQL_DBNAME,MYSQL_DBPORT,cfd,buffer,data);
						break;
					}
					case 0x03:
					{
						addFriendHandler(fp,ip,MYSQL_USRNAME,MYSQL_USRPASS,MYSQL_DBNAME,MYSQL_DBPORT,cfd,buffer);
						break;
					}
					case 0x04:
					{
						p2pChatHandler(fp,ip,MYSQL_USRNAME,MYSQL_USRPASS,MYSQL_DBNAME,MYSQL_DBPORT,cfd,buffer,data);
						break;
					}
					case 0x05:
					{
						addGroupHandler(fp,ip,MYSQL_USRNAME,MYSQL_USRPASS,MYSQL_DBNAME,MYSQL_DBPORT,cfd,buffer,data);
						break;
					}
					case 0x06:
					{
						groupChatHandler(fp,ip,MYSQL_USRNAME,MYSQL_USRPASS,MYSQL_DBNAME,MYSQL_DBPORT,cfd,buffer,data);
						break;
					}
					case 0x07:
					{
						logoutHandler(fp,ip,MYSQL_USRNAME,MYSQL_USRPASS,MYSQL_DBNAME,MYSQL_DBPORT,cfd,buffer);
						break;
					}
					case 0x08:
					{
						queryGroupMemberListHandler(fp,ip,MYSQL_USRNAME,MYSQL_USRPASS,MYSQL_DBNAME,MYSQL_DBPORT,cfd,buffer,data);
						break;
					}
					default:
					{
						break;
					}			
				}
			}
		}
	}
}

void 
heartbeatService 
(FILE * fp, char * ip)
{
	heartbeatHandler(fp,ip,MYSQL_USRNAME,MYSQL_USRPASS,MYSQL_DBNAME,MYSQL_DBPORT);
}

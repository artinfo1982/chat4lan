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
#include <limits.h>

/*
* register:
* client --> (flag[1], username_len[1], password_len[1], username[?], password[?])
* server --> (flag[1], userid[1] or space[1])
*
* login:
* client --> (flag[1], password_len[1], userid[1], password[?])
* server --> (flag[1], list_size[1], list[?])
*/

#define MAX_USER_NUMBER 		100
// general space use to fill message body
#define GENERAL_SPACE			"\x00"	// 0000 0000
// login success
#define SVR_RSP_LON_SUC 			"\x01"	// 0000 0001
// register success
#define SVR_RSP_REG_SUC 			"\x02"	// 0000 0010
// register users more than max user number
#define SVR_RSP_REG_ERR_MAX_USR "\x81"	// 1000 0001
// the same user register twice
#define SVR_RSP_REG_ERR_REP		"\x82"	// 1000 0010	
// authentication failed
#define SVR_RSP_LON_ERR_REP		"\x83"	// 1000 0011
// user not in memory database
#define SVR_RSP_LON_ERR_NOT_EXI	"\x84"	// 1000 0100

typedef struct _user_info
{
	int userid;
	char username[CHAR_MAX];
	char password[CHAR_MAX];
	char friend_list[CHAR_MAX];
}user_info;

//global user id, start from 0
int g_user_id;
int g_real_user_num;
user_info ui[MAX_USER_NUMBER];

int authentication(int userid, char * input_pass)
{
	int result = 0, i, flag = 0;
	// query userid from memory database
	for (i = 0; i < g_real_user_num; i++)
	{
		// found user in memory databases
		if (userid == ui[i].userid)
		{
			flag = 1;
			// check password
			if (0 == strcmp(input_pass, ui[i].password))
				return 0;
			else
				return -1;
		}			
	}
	// can not find user in memory database
	if (0 == flag)
		return 1;
}

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
	if (dup2(fd, STDERR_FILENO) == -1)
	{
		printf("dup2 STDERR_FILENO failed\n");
		exit(1);
	}
	close(fd);

	int sfd, cfd, efd, flag, rep, nfds, i;
	char buffer[8192], data[64];
    	struct epoll_event sev,cev, events[256];
	struct sockaddr_in servAddr;
	struct sockaddr_in cliAddr;
	
    	memset(&servAddr, 0, sizeof(servAddr));
	memset(&cliAddr, 0, sizeof(cliAddr));
	socklen_t addrlen = sizeof(cliAddr);
    	servAddr.sin_family = AF_INET;
    	servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    	servAddr.sin_port = htons((unsigned short)atoi(argv[2]));	

	int username_len, userid_len, password_len;
	char username[CHAR_MAX], password[CHAR_MAX];
	int userid;

	g_user_id = 2;
	memset(&ui, 0x0, sizeof(ui));

	// to be delete
	ui[0].userid = 0;
	strncpy(ui[0].username, "AAA", 3);
	strncpy(ui[0].password, "aaa", 3);
	ui[0].friend_list[0] = (char)'1';
	ui[0].friend_list[1] = (char)'2';
	ui[1].userid = 1;
	strncpy(ui[1].username, "BBB", 3);
	strncpy(ui[1].password, "bbb", 3);
	ui[1].friend_list[0] = (char)'0';
	ui[1].friend_list[1] = (char)'2';
	
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
					printf("accept failed\n");
					continue;
				}
				memset(buffer, 0x0, sizeof(buffer));
				memset(data, 0x0, sizeof(data));
				if (recv(cfd, buffer, 8192, 0) < 0)
				{
					printf("recv failed\n");
					close(cfd);
					continue;
				}
				switch (buffer[0])
				{
					//login
					case 0x01:
						password_len = (int)buffer[1];
						memset(password, 0x0, sizeof(password));
						userid = (int)buffer[2];
						strncpy(password, buffer + 3 + userid_len, password_len);				
						password[password_len] = '\0';
						// user not found in memory dataspace
						if (authentication(userid, password) > 0)
						{
							sprintf(data, "%s%s", SVR_RSP_LON_ERR_NOT_EXI, GENERAL_SPACE);
							send(cfd, data, 2, 0);
							break;
						}
						// authentication failed
						else if (authentication(userid, password) < 0)
						{
							sprintf(data, "%s%s", SVR_RSP_LON_ERR_REP, GENERAL_SPACE);
							send(cfd, data, 2, 0);
							break;
						}
						else
						{
							// list
							sprintf(data, "%s%s", SVR_RSP_LON_SUC, GENERAL_SPACE);
							send(cfd, data, 2, 0);
							break;
						}
						break;
					//register
					case 0x02:
						if (g_user_id > MAX_USER_NUMBER)
						{
							sprintf(data, "%s%s", SVR_RSP_REG_ERR_MAX_USR, GENERAL_SPACE);
							send(cfd, data, 2, 0);
							break;
						}
						ui[g_user_id].userid = g_user_id;
						username_len = (int)buffer[1];
						password_len = (int)buffer[2];
						memset(username, 0x0, sizeof(username));
						strncpy(username, buffer + 3, username_len);
						username[username_len] = '\0';				
						flag = 0;
						for (i = 0; i <= g_user_id; i++)
						{
							if (0 == strcmp(username, ui[i].username))
							{
								flag = 1;
								break;
							}
						}
						if (1 == flag)
						{
							sprintf(data, "%s%s", SVR_RSP_REG_ERR_REP, GENERAL_SPACE);
							send(cfd, SVR_RSP_REG_ERR_REP, 2, 0);
							break;
						}
						strncpy(ui[g_user_id].username, buffer + 3, username_len);
						ui[g_user_id].username[username_len] = '\0';
						strncpy(ui[g_user_id].password, buffer + 3 + username_len, password_len);				
						ui[g_user_id].password[password_len] = '\0';
						sprintf(data, "%s%c", SVR_RSP_REG_SUC, ui[g_user_id].userid);
						send(cfd, data, strlen(data), 0);
						g_user_id++;
						g_real_user_num = g_user_id;
						break;
					default:
						break;
				}
			}
			close(cfd);
		}
	}
	return 0;
}

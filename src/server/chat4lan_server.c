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

#define MAX_USER_NUMBER 		CHAR_MAX
// login success
#define SVR_RSP_LON_SUC 			'\x01'	// 0000 0001
// register success
#define SVR_RSP_REG_SUC 				'\x02'	// 0000 0010
// add friend success
#define SVR_RSP_ADD_FRI_SUC			'\x03'	// 0000 0011
// register users more than max user number
#define SVR_RSP_REG_ERR_MAX_USR 	'\x81'	// 1000 0001
// the same user register twice
#define SVR_RSP_REG_ERR_REP			'\x82'	// 1000 0010	
// authentication failed
#define SVR_RSP_LON_ERR_REP			'\x83'	// 1000 0011
// user not in memory database
#define SVR_RSP_LON_ERR_NOT_EXI		'\x84'	// 1000 0100
// friend not in memory database
#define SVR_RSP_ADD_FRI_ERR_NOT_EXI	'\x85'	// 1000 0101
// friend is already added
#define SVR_RSP_ADD_FRI_ERR_AGN		'\x86'	// 1000 0110

#define USER_STATUS_ONLINE			'\xF0'	// 1111 0000
#define USER_STATUS_OFFLINE			'\xFF'	// 1111 1111

typedef struct _user_info
{
	int 		userid;
	char 	username [CHAR_MAX];
	char 	password [CHAR_MAX];
	int		friend_num;
	int		friend_list [MAX_USER_NUMBER];
	char 	user_status;
}user_info;

//global user id, start from 0
int g_user_id;
int g_real_user_num;
user_info ui[MAX_USER_NUMBER];

int userid_generator()
{
	int userid;
	if (g_user_id > MAX_USER_NUMBER)
		return -1;		
	else
	{
		userid = g_user_id ++;
		g_real_user_num = g_user_id;
		return userid;
	}
}

int authentication(int userid, char * input_pass)
{
	int i, flag = 0;
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

int add_friend(int selfID, int friendID)
{
	int i, j, flag = 0;
	// query friendID from memory database
	for (i = 0; i < g_real_user_num; i++)
	{
		// found friend in memory databases
		if (friendID == ui[i].userid)
		{
			flag = 1;
			// check if this friendID has already been added
			for (j = 0; j< ui[selfID].friend_num; j++)
			{
				// this friendID has been added
				if (friendID == ui[selfID].friend_list[j])
					return -1;
			}
			// add friend to user's friend list
			ui[selfID].friend_list[(ui[selfID].friend_num ++)] = friendID;
			return 0;
		}			
	}
	// can not find friend in memory database
	if (0 == flag)
		return 1;
}

void send_flag_msg(int sock, char flag)
{
	char * index = & flag;
	send(sock, index, 1, 0);
	close(sock);
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

	int sfd, cfd, efd, flag, rep, nfds, i, j;
	char buffer[1024], data[1024];
    	struct epoll_event sev,cev, events[256];
	struct sockaddr_in servAddr;
	struct sockaddr_in cliAddr;
	
    	memset(&servAddr, 0, sizeof(servAddr));
	memset(&cliAddr, 0, sizeof(cliAddr));
	socklen_t addrlen = sizeof(cliAddr);
    	servAddr.sin_family = AF_INET;
    	servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    	servAddr.sin_port = htons((unsigned short)atoi(argv[2]));	

	char login_tmp[CHAR_MAX], register_tmp[CHAR_MAX], add_fri_tmp[CHAR_MAX];
	char * index = NULL;
	int userID, uid1, uid2, friendID, length;

	g_user_id = 0;
	memset(&ui, 0x0, sizeof(ui));
	
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
				if (recv(cfd, buffer, 1024, 0) < 0)
				{
					printf("recv failed\n");
					close(cfd);
					continue;
				}
				switch (buffer[0])
				{
					//login
					case 0x01:
						// get userid
						memcpy(login_tmp, buffer + 3, (int)buffer[1]);
						login_tmp[((int)buffer[1])] = '\0';
						userID= atoi(login_tmp);
						// get password
						memcpy(login_tmp, buffer + 3 + (int)buffer[1], (int)buffer[2]);
						login_tmp[((int)buffer[2])] = '\0';
						// user not found in memory dataspace
						if (authentication(userID, login_tmp) > 0)
						{
							send_flag_msg(cfd, SVR_RSP_LON_ERR_NOT_EXI);
							break;
						}
						// authentication failed
						else if (authentication(userID, login_tmp) < 0)
						{
							send_flag_msg(cfd, SVR_RSP_LON_ERR_REP);
							break;
						}
						// login success
						else
						{
							ui[userID].user_status = USER_STATUS_ONLINE;
							data[0] = SVR_RSP_LON_SUC;
							snprintf(login_tmp, CHAR_MAX, "%d", ui[userID].friend_num);
							length = strlen(login_tmp);
							data[1] = length;
							index = data + 2;
							memcpy(index, login_tmp, length);
							index += length;
							for (j = 0; j < ui[userID].friend_num; j++)
							{
								// get friend name by userid
								snprintf(login_tmp, CHAR_MAX, "%s", ui[(ui[userID].friend_list[j])].username);
								length = strlen(login_tmp);
								data[index - data] = length;
								index ++;
								memcpy(index, login_tmp, length);
								index += length;
								// get friend status by userid
								data[index - data] = ui[(ui[userID].friend_list[j])].user_status;
								index ++;
							}
							data[index - data] = '\0';
							send(cfd, data, strlen(data), 0);
							break;
						}
						break;
					//register
					case 0x02:
						// more than max user
						if ((uid1 = userid_generator()) < 0)
						{
							send_flag_msg(cfd, SVR_RSP_REG_ERR_MAX_USR);
							break;
						}
						ui[uid1].userid = uid1;
						memcpy(register_tmp, buffer + 3, (int)buffer[1]);
						register_tmp[((int)buffer[1])] = '\0';
						flag = 0;
						for (j = 0; j < g_real_user_num; j++)
						{
							if (0 == strcmp(register_tmp, ui[j].username))
							{
								flag = 1;
								break;
							}
						}
						if (1 == flag)
						{
							send_flag_msg(cfd, SVR_RSP_REG_ERR_REP);
							break;
						}
						memcpy(ui[uid1].username, buffer + 3, (int)buffer[1]);
						ui[uid1].username[((int)buffer[1])] = '\0';
						memcpy(ui[uid1].password, buffer + 3 + (int)buffer[1], (int)buffer[2]);			
						ui[uid1].password[(int)buffer[2]] = '\0';
						ui[uid1].user_status = USER_STATUS_OFFLINE;
						ui[uid1].friend_num = 0;
						data[0] = SVR_RSP_REG_SUC;
						index = data + 1;
						snprintf(register_tmp, CHAR_MAX, "%d", ui[uid1].userid);
						length = strlen(register_tmp);
						memcpy(index, register_tmp, length);
						index += length;
						data[index - data] = '\0';			
						send(cfd, data, strlen(data), 0);
						break;
					// add friend
					case 0x03:
						memcpy(add_fri_tmp, buffer + 3, (int)buffer[1]);
						add_fri_tmp[((int)buffer[1])] = '\0';
						uid2 = atoi(add_fri_tmp);
						memcpy(add_fri_tmp, buffer + 3 + (int)buffer[1], (int)buffer[2]);
						add_fri_tmp[((int)buffer[2])] = '\0';			
						friendID = atoi(add_fri_tmp);
						// friendID bas been added
						if (add_friend(uid2, friendID) < 0)
						{
							send_flag_msg(cfd, SVR_RSP_ADD_FRI_ERR_AGN);
							break;
						}
						// friendID is not in memory database
						else if (add_friend(uid2, friendID) > 0)
						{
							send_flag_msg(cfd, SVR_RSP_ADD_FRI_ERR_NOT_EXI);
							break;
						}
						// add friend success
						else
						{
							send_flag_msg(cfd, SVR_RSP_ADD_FRI_SUC);
							break;
						}
						break;
					default:
					{
						close(cfd);
						break;
					}		
				}
			}
			close(cfd);
		}
	}
	return 0;
}

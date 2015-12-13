#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

typedef void (*mainServiceType)(FILE *, char *, char *);
typedef void (*heartbeatServiceType)(FILE *, char *);

void 
setDaemon 
(FILE * fp);

void 
createMainProcess 
(FILE * fp, mainServiceType mst, char * ip, char * port);

void 
createHeartbeatProcess 
(FILE * fp, heartbeatServiceType hst, char * ip);

void 
initServerAddr 
(struct sockaddr_in* servAddr, char * serverIP, char * serverPort);

void 
initClientAddr 
(struct sockaddr_in* cliAddr);

int 
initListenSock 
(FILE * fp, struct sockaddr_in* servAddr);

int 
initEpoll 
(FILE * fp, int sfd, struct epoll_event* sev);

int 
epollWait 
(FILE * fp, int efd, int sfd, struct epoll_event* events);

void 
sendFlagMsg 
(int sock, char flag);

int 
sendRecvHeartbeat 
(int sock);

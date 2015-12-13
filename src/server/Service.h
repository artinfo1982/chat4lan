#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Constants.h"

int 
loginHandler 
(FILE * fp, char * dbIP, char * dbUserName, char * dbPass, char * dbName, int dbPort, int sock, char * buffer, char * data);

int 
registerHandler 
(FILE * fp, char * dbIP, char * dbUserName, char * dbPass, char * dbName, int dbPort, int sock, char * buffer, char * data);

int 
addFriendHandler 
(FILE * fp, char * dbIP, char * dbUserName, char * dbPass, char * dbName, int dbPort, int sock, char * buffer, char * data);

int 
p2pChatHandler 
(FILE * fp, char * dbIP, char * dbUserName, char * dbPass, char * dbName, int dbPort, int sock, char * buffer, char * data);

int 
addGroupHandler 
(FILE * fp, char * dbIP, char * dbUserName, char * dbPass, char * dbName, int dbPort, int sock, char * buffer, char * data);

int 
groupChatHandler 
(FILE * fp, char * dbIP, char * dbUserName, char * dbPass, char * dbName, int dbPort, int sock, char * buffer, char * data);

int 
logoutHandler 
(FILE * fp, char * dbIP, char * dbUserName, char * dbPass, char * dbName, int dbPort, int sock, char * buffer, char * data);

int 
queryGroupMemberListHandler 
(FILE * fp, char * dbIP, char * dbUserName, char * dbPass, char * dbName, int dbPort, int sock, char * buffer, char * data);

void 
heartbeatHandler 
(FILE * fp, char * dbIP, char * dbUserName, char * dbPass, char * dbName, int dbPort);

void 
markUsrOnline2Offline 
(FILE * fp, char * dbIP, char * dbUserName, char * dbPass, char * dbName, int dbPort, int usrID);

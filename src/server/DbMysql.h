#include <stdio.h>
#include <mysql/mysql.h>
#include <limits.h>
#include <string.h>

int 
mysqlInsUpdDel 
(FILE * fp, char * ip, char * username, char * password, char * dbName, int dbPort, char * sql);

int 
mysqlQueryCount 
(FILE * fp, char * ip, char * username, char * password, char * dbName, int dbPort, char * sql);

int 
mysqlQuerySimple 
(FILE * fp, char * ip, char * username, char * password, char * dbName, int dbPort, char * sql, char * buffer);

MYSQL_RES * 
mysqlQueryGetRes 
(FILE * fp, char * ip, char * username, char * password, char * dbName, int dbPort, char * sql);

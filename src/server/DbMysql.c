#include "DbMysql.h"
#include "Log.h"

// insert, update and delete operation of mysql
int 
mysqlInsUpdDel 
(FILE * fp, char * ip, char * username, char * password, char * dbName, int dbPort, char * sql)
{
	struct timeval tv;
	struct tm tim;
	MYSQL * conn = NULL;

	conn = mysql_init(NULL);
	if (! conn)
	{
		ERROR_LOG(fp, tv, tim, "init mysql connector failed\n");
		return -1;
	}
	if(! mysql_real_connect(conn, ip, username, password, dbName, dbPort, NULL, 0))
	{
		ERROR_LOG(fp, tv, tim, "connect to mysql failed\n");
		mysql_close(conn);
		return -1;
	}
	if (mysql_real_query(conn, "set names utf8", 14))
	{
		ERROR_LOG(fp, tv, tim, "mysql set names utf8 failed\n");
		mysql_close(conn);
		return -1;
	}
	if (mysql_real_query(conn, sql, strlen(sql)))
	{
		ERROR_LOG(fp, tv, tim, "mysql insert, update or delete failed, sql=%s\n", sql);
		mysql_close(conn);
		return -1;
	}
	mysql_close(conn);	
	return 0;
}

// only return query results count
int 
mysqlQueryCount 
(FILE * fp, char * ip, char * username, char * password, char * dbName, int dbPort, char * sql)
{
	struct timeval tv;
	struct tm tim;
	MYSQL * conn = NULL;
	MYSQL_RES * res = NULL;
	MYSQL_ROW rows;
	
	char buffer[CHAR_MAX];
	int count;

	conn = mysql_init(NULL);
	if (! conn)
	{
		ERROR_LOG(fp, tv, tim, "init mysql connector failed\n");
		return -1;
	}
	if(! mysql_real_connect(conn, ip, username, password, dbName, dbPort, NULL, 0))
	{
		ERROR_LOG(fp, tv, tim, "connect to mysql failed\n");
		mysql_close(conn);
		return -1;
	}
	if (mysql_real_query(conn, "set names utf8", 14))
	{
		ERROR_LOG(fp, tv, tim, "mysql set names utf8 failed\n");
		mysql_close(conn);
		return -1;
	}
	if (mysql_real_query(conn, sql, strlen(sql)))
	{
		ERROR_LOG(fp, tv, tim, "query from mysql failed, sql=%s\n", sql);
		mysql_close(conn);
		return -1;
	}
	res = mysql_store_result(conn);
	if (! res)
	{
		ERROR_LOG(fp, tv, tim, "store results from mysql server to local failed\n");
		mysql_close(conn);
		return -1;
	}

	rows = mysql_fetch_row(res);	
	if (0 == rows)
		return -1;

	memcpy(buffer, rows[0], strlen(rows[0]));
	count = atoi(buffer);

	mysql_free_result(res);
	mysql_close(conn);
	
	return count;
}

// return query result only one row
int 
mysqlQuerySimple 
(FILE * fp, char * ip, char * username, char * password, char * dbName, int dbPort, char * sql, char * buffer)
{
	struct timeval tv;
	struct tm tim;
	MYSQL * conn = NULL;
	MYSQL_RES * res = NULL;
	MYSQL_ROW rows;
	
	conn = mysql_init(NULL);
	if (! conn)
	{
		ERROR_LOG(fp, tv, tim, "init mysql connector failed\n");
		return -1;
	}
	if(! mysql_real_connect(conn, ip, username, password, dbName, dbPort, NULL, 0))
	{
		ERROR_LOG(fp, tv, tim, "connect to mysql failed\n");
		mysql_close(conn);
		return -1;
	}
	if (mysql_real_query(conn, "set names utf8", 14))
	{
		ERROR_LOG(fp, tv, tim, "mysql set names utf8 failed\n");
		mysql_close(conn);
		return -1;
	}
	if (mysql_real_query(conn, sql, strlen(sql)))
	{
		ERROR_LOG(fp, tv, tim, "query from mysql failed, sql=%s\n", sql);
		mysql_close(conn);
		return -1;
	}
	res = mysql_store_result(conn);
	if (! res)
	{
		ERROR_LOG(fp, tv, tim, "store results from mysql server to local failed\n");
		mysql_close(conn);
		return -1;
	}

	rows = mysql_fetch_row(res);	
	if (0 == rows)
		return -1;

	memcpy(buffer, rows[0], strlen(rows[0]));
	buffer[strlen(rows[0])] = '\0';

	mysql_free_result(res);
	mysql_close(conn);
	
	return 0;
}


MYSQL_RES * 
mysqlQueryGetRes 
(FILE * fp, char * ip, char * username, char * password, char * dbName, int dbPort, char * sql)
{
	struct timeval tv;
	struct tm tim;
	MYSQL * conn = NULL;
	MYSQL_RES * res = NULL;

	conn = mysql_init(NULL);
	if (! conn)
	{
		ERROR_LOG(fp, tv, tim, "init mysql connector failed\n");
		return NULL;
	}
	if(! mysql_real_connect(conn, ip, username, password, dbName, dbPort, NULL, 0))
	{
		ERROR_LOG(fp, tv, tim, "connect to mysql failed\n");
		mysql_close(conn);
		return NULL;
	}
	if (mysql_real_query(conn, "set names utf8", 14))
	{
		ERROR_LOG(fp, tv, tim, "mysql set names utf8 failed\n");
		mysql_close(conn);
		return NULL;
	}
	if (mysql_real_query(conn, sql, strlen(sql)))
	{
		ERROR_LOG(fp, tv, tim, "query from mysql failed, sql=%s\n", sql);
		mysql_close(conn);
		return NULL;
	}
	res = mysql_store_result(conn);
	if (! res)
	{
		ERROR_LOG(fp, tv, tim, "store results from mysql server to local failed\n");
		mysql_close(conn);
		return NULL;
	}
	mysql_close(conn);
	return res;
}

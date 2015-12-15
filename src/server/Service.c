#include "Service.h"
#include "DbMysql.h"
#include "Log.h"
#include "Common.h"

int 
loginHandler 
(FILE * fp, char * dbIP, char * dbUserName, char * dbPass, char * dbName, int dbPort, int sock, char * buffer, char * data)
{
	struct timeval tv;
	struct tm tim;
	char sql[512];
	char temp[CHAR_MAX];
	int usrID;
	char usrName[CHAR_MAX];
	char usrPass[CHAR_MAX];
	char usrLocalIP[16];
	int usrLocalPort;
	int friendNum;
	int groupNum;
	int onlineNum;
	int totalNum;
	int length;
	int i;
	char * index = NULL;
	MYSQL_RES * res = NULL;
	MYSQL_ROW row;
	
	// get usrID
	memcpy(temp, buffer + 5, (int)buffer[1]);
	temp[((int)buffer[1])] = '\0';
	usrID = atoi(temp);
	// get password
	memcpy(usrPass, buffer + 5 + (int)buffer[1], (int)buffer[2]);
	usrPass[((int)buffer[2])] = '\0';
	// check usrID if is not exist
	sprintf(sql, "%s%d", "select count(1) from usr_info where usr_id=", usrID);
	sql[43 + (int)*(buffer + 1)] = '\0';
	if (0 == mysqlQueryCount(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
	{
		sendFlagMsg(sock, SVR_RSP_LON_ERR_NOT_EXI);
		return 1;
	}
	// check usr if is already login
	sprintf(sql, "%s%d%s", "select count(1) from usr_info where usr_id=", usrID, " and usr_status='1'");
	sql[62 + (int)*(buffer + 1)] = '\0';
	if (0 != mysqlQueryCount(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
	{
		sendFlagMsg(sock, SVR_RSP_USR_LOG_IN_REP);
		return 1;
	}
	// authentication
	sprintf(sql, "%s%d%s%s%s", "select count(1) from usr_info where usr_id=", usrID, " and usr_password='", usrPass, "'");
	sql[63 + (int)*(buffer + 1) + ((int)buffer[2])] = '\0';
	if (0 == mysqlQueryCount(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
	{
		sendFlagMsg(sock, SVR_RSP_LON_ERR_REP);
		return 1;
	}
	// get local ip
	memcpy(usrLocalIP, buffer + 5 + (int)buffer[1] + (int)buffer[2], (int)buffer[3]);
	usrLocalIP[((int)buffer[3])] = '\0';
	// get local port
	memcpy(temp, buffer + 5 + (int)buffer[1] + (int)buffer[2] + (int)buffer[3], (int)buffer[4]);
	temp[((int)buffer[4])] = '\0';
	usrLocalPort = atoi(temp);
	// login success, mark usr online, and update usr_info in database
	sprintf(sql, "%s%c%s%s%s%d%s%d", "update usr_info set usr_status=", USER_STATUS_ONLINE, 
		",usr_local_ip='", usrLocalIP, "',usr_local_port=", usrLocalPort, " where usr_id=", usrID);
	sql[78 + ((int)buffer[1]) + ((int)buffer[3]) +  ((int)buffer[4])] = '\0';
	if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
	{
		sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
		ERROR_LOG(fp, tv, tim, "login, mysql is unreachable when update usr_info table\n");
		return 1;
	}
	// login success, update group_online_member_num of group_info
	sprintf(sql, "%s%d", "update group_info g,usr_group_info u set g.group_online_member_num=g.group_online_member_num+1 where u.group_id=g.group_id and u.usr_id=", usrID);
	sql[136 + ((int)buffer[1])] = '\0';
	if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
	{
		sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
		ERROR_LOG(fp, tv, tim, "login, mysql is unreachable when update group_info table\n");
		return 1;
	}
	data[0] = SVR_RSP_LON_IN_SUC;
	// query usr_name, usr_friend_num and usr_group_num
	sprintf(sql, "%s%d", "select usr_name,usr_friend_num,usr_group_num from usr_info where usr_id=", usrID);
	sql[72 + ((int)buffer[1])] = '\0';
	res = mysqlQueryGetRes(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql);
	if ( ! res )
	{
		ERROR_LOG(fp, tv, tim, "internal error, login query usr_name, usr_friend_num and usr_group_num return empty, sql=%s", sql);
		return 1;
	}
	row = mysql_fetch_row(res);
	mysql_free_result(res);
	// put usrName into data
	memcpy(usrName, row[0], strlen(row[0]));
	usrName[strlen(row[0])] = '\0';
	length = strlen(usrName);
	data[1] = length;
	index = data + 2;
	memcpy(index, usrName, length);
	index += length;
	//put friendNum into data
	memcpy(temp, row[1], strlen(row[1]));
	temp[strlen(row[1])] = '\0';
	length = strlen(temp);
	data[index - data] = length;
	index ++;
	memcpy(index, temp, length);
	index += length;
	friendNum = atoi(temp);
	// put groupNum into data
	memcpy(temp, row[2], strlen(row[2]));
	temp[strlen(row[2])] = '\0';
	length = strlen(temp);
	data[index - data] = length;
	index ++;
	memcpy(index, temp, length);
	index += length;
	groupNum = atoi(temp);
	// query friend list of this user
	sprintf(sql, "%s%d%s", "select usr_friend_id,usr_name,usr_status from usr_friend_info f,usr_info i where f.usr_id=", usrID, " and f.usr_friend_id=i.usr_id");
	sql[119 + ((int)buffer[1])] = '\0';
	res = mysqlQueryGetRes(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql);
	if ( ! res )
	{
		ERROR_LOG(fp, tv, tim, "internal error, login query friend list return empty, sql=%s", sql);
		return 1;
	}
	for (i = 0; i < friendNum; i++)
	{
		row = mysql_fetch_row(res);
		// get friend userid
		length = strlen(row[0]);
		data[index - data] = length;
		index ++;
		memcpy(index, row[0], length);
		index += length;
		// get friend name
		length = strlen(row[1]);
		data[index - data] = length;
		index ++;
		memcpy(index, row[1], length);
		index += length;
		// get friend status
		memcpy(index, row[2], 1);
		index ++;
	}
	mysql_free_result(res);
	// query group list of this user
	sprintf(sql, "%s%d", "select i.group_id,group_name,group_online_member_num,group_member_num from group_info i,usr_group_info u where i.group_id=u.group_id and u.usr_id=", usrID);
	sql[146 + ((int)buffer[1])] = '\0';
	res = mysqlQueryGetRes(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql);	
	if ( ! res )
	{
		ERROR_LOG(fp, tv, tim, "internal error, login query group list return empty, sql=%s", sql);
		return 1;
	}
	for (i = 0; i < groupNum; i++)
	{
		row = mysql_fetch_row(res);
		// get group id
		length = strlen(row[0]);
		data[index - data] = length;
		index ++;
		memcpy(index, row[0], length);
		index += length;
		// get group name
		length = strlen(row[1]);
		data[index - data] = length;
		index ++;
		memcpy(index, row[1], length);
		index += length;
		// get group online member number/total member number
		onlineNum = atoi(row[2]);
		totalNum= atoi(row[3]);
		snprintf(temp, CHAR_MAX, "%d/%d", onlineNum, totalNum);
		length = strlen(temp);
		data[index - data] = length;
		index ++;
		memcpy(index, temp, length);
		index += length;
	}
	mysql_free_result(res);
	data[index - data] = '\0';
	send(sock, data, strlen(data), 0);
	INFO_LOG(fp, tv, tim, "usr login successfully, usrID=%d, usrName=%s, friendNum=%d, groupNum=%d\n",usrID, usrName, friendNum, groupNum);
	return 0;
}

int 
registerHandler 
(FILE * fp, char * dbIP, char * dbUserName, char * dbPass, char * dbName, int dbPort, int sock, char * buffer, char * data)
{
	struct timeval tv;
	struct tm tim;
	char sql[512];
	char usrID[16];
	char usrName[CHAR_MAX];
	char usrPass[CHAR_MAX];
	char usrStatus;
	char * index = NULL;
	int length;

	//check if more than max users
	sprintf(sql, "%s", "select count(1) from usr_info");
	sql[29] = '\0';
	if (mysqlQueryCount(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql) >= MAX_USER_NUMBER)
	{
		sendFlagMsg(sock, SVR_RSP_REG_ERR_MAX_USR);
		WARN_LOG(fp, tv, tim, "max user number is reached in this system, can not add user, current user number=%d\n", MAX_USER_NUMBER);
		return 1;
	}
	//get usrName
	memcpy(usrName, buffer + 3, (int)buffer[1]);
	usrName[((int)buffer[1])] = '\0';
	memcpy(usrPass, buffer + 3 + (int)buffer[1], (int)buffer[2]);			
	usrPass[(int)buffer[2]] = '\0';
	usrStatus = USER_STATUS_OFFLINE;
	// check user is exist or not
	sprintf(sql, "%s%s'", "select count(1) from usr_info where usr_name='", usrName);
	sql[47 + ((int)buffer[1])] = '\0';
	if (0 != mysqlQueryCount(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
	{
		sendFlagMsg(sock, SVR_RSP_REG_ERR_REP);
		return 1;
	}
	//put usr info into db
	sprintf(sql, "%s%s%s%s%s%d%s", "insert into usr_info"
		"(usr_name,usr_password,usr_friend_num,usr_group_num,usr_status,usr_local_ip,usr_local_port) "
		"values ('", usrName, "','", usrPass, "',0,0,",USER_STATUS_OFFLINE,",'',0)");
	sql[137 + ((int)buffer[1]) +  ((int)buffer[2])] = '\0';
	if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
	{
		sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
		ERROR_LOG(fp, tv, tim, "register, mysql is unreachable when insert into usr_info table\n");
		return 1;
	}
	//get usrID from database
	sprintf(sql, "%s%s%s", "select usr_id from usr_info where usr_name='", usrName, "'");
	sql[45 + ((int)buffer[1])] = '\0';
	if (0 != mysqlQuerySimple(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql, usrID))
	{
		sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
		ERROR_LOG(fp, tv, tim, "register, mysql is unreachable when get usrID from usr_info table\n");
		return 1;
	}
	//reply msg to client
	data[0] = SVR_RSP_REG_SUC;
	index = data + 1;
	length = strlen(usrID);
	memcpy(index, usrID, length);
	index += length;
	data[index - data] = '\0';			
	send(sock, data, strlen(data), 0);
	INFO_LOG(fp, tv, tim, "a new user is added successfully into system, usrID=%s, usrName=%s\n",usrID, usrName);
	return 0;
}

int 
addFriendHandler 
(FILE * fp, char * dbIP, char * dbUserName, char * dbPass, char * dbName, int dbPort, int sock, char * buffer)
{
	struct timeval tv;
	struct tm tim;
	char sql[512];
	char usrID[16];
	char friendID[16];

	memcpy(usrID, buffer + 3, (int)buffer[1]);
	usrID[((int)buffer[1])] = '\0';
	memcpy(friendID, buffer + 3 + (int)buffer[1], (int)buffer[2]);
	friendID[((int)buffer[2])] = '\0';

	// check if friendID is not in database
	sprintf(sql, "%s%s", "select count(1) from usr_info where usr_id=", friendID);
	sql[43 + ((int)buffer[2])] = '\0';
	if (0 == mysqlQueryCount(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
	{
		sendFlagMsg(sock, SVR_RSP_ADD_FRI_ERR_NOT_EXI);
		return 1;
	}
	// check if this friend has already been added
	sprintf(sql, "%s%s%s%s", "select count(1) from usr_friend_info where usr_id=", usrID, " and usr_friend_id=", friendID);
	sql[69 + ((int)buffer[1]) + ((int)buffer[2])] = '\0';
	if (0 != mysqlQueryCount(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
	{
		sendFlagMsg(sock, SVR_RSP_ADD_FRI_ERR_AGN);
		return 1;
	}
	// update usr_friend_info table
	sprintf(sql, "%s%s,%s)", "insert into usr_friend_info(usr_id,usr_friend_id) values(", usrID, friendID);
	sql[59 + ((int)buffer[1]) +  ((int)buffer[2])] = '\0';
	if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
	{
		sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
		ERROR_LOG(fp, tv, tim, "addFriend, mysql is unreachable when update usr_friend_info table\n");
		return 1;
	}
	// update usr_friend_num in usr_info table
	sprintf(sql, "%s%s", "update usr_info set usr_friend_num=usr_friend_num+1 where usr_id=", usrID);
	sql[65 + ((int)buffer[1])] = '\0';
	if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
	{
		sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
		ERROR_LOG(fp, tv, tim, "addFriend, mysql is unreachable when update usr_friend_num in usr_info table\n");
		return 1;
	}
	// add friend success response
	sendFlagMsg(sock, SVR_RSP_ADD_FRI_SUC);
	INFO_LOG(fp, tv, tim, "add friend successfully, usrID=%s, friendID=%s\n",usrID, friendID);
	return 0;
}

int 
p2pChatHandler 
(FILE * fp, char * dbIP, char * dbUserName, char * dbPass, char * dbName, int dbPort, int sock, char * buffer, char * data)
{
	struct timeval tv;
	struct tm tim;
	char sql[512];
	char temp[CHAR_MAX];
	int usrID;
	int friendID;
	char status;
	char friendIP[16];
	int sfd = -1;
	int length;
	struct sockaddr_in friendAddr;
	char * index = NULL;
	MYSQL_RES * res = NULL;
	MYSQL_ROW row;

	memset(&friendAddr, 0, sizeof(struct sockaddr_in));
    	friendAddr.sin_family = AF_INET;

	// get sender usrID
	memcpy(temp, buffer + 4, (int)buffer[1]);
	temp[((int)buffer[1])] = '\0';
	usrID = atoi(temp);
	// get friendID
	memcpy(temp, buffer + 4 + (int)buffer[1], (int)buffer[2]);
	temp[((int)buffer[2])] = '\0';
	friendID = atoi(temp);
	// make msg prepare to send to friend
	data[0] = SEND_RECV_MSG_P2P;
	data[1] = buffer[1];
	data[2] = buffer[3];
	index = data + 3;
	memcpy(index, buffer + 4, (int)buffer[1]);
	index += (int)buffer[1];
	memcpy(index, buffer + 4 + (int)buffer[1] + (int)buffer[2], (int)buffer[3]);
	index += (int)buffer[3];
	data[index - data] = '\0';
	// query friend status, only online should do send, 
	// if friend is offline or send failed, should restore msg
	sprintf(sql, "%s%d", "select usr_status from usr_info where usr_id=", friendID);
	sql[45 + ((int)buffer[2])] = '\0';
	if (0 != mysqlQuerySimple(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql, &status))
	{
		sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
		ERROR_LOG(fp, tv, tim, "p2pChat, mysql is unreachable when query friend status from usr_info table\n");
		return 1;
	}	
	// friend is online
	if ('1' == status)
	{
		// query friend ip and port from database
		sprintf(sql, "%s%d", "select usr_local_ip,usr_local_port from usr_info where usr_id=", friendID);
		sql[62 + ((int)buffer[2])] = '\0';
		res = mysqlQueryGetRes(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql);
		if ( ! res )
		{
			ERROR_LOG(fp, tv, tim, "internal error, p2pchat query friend ip and port from database return empty, sql=%s\n", sql);
			return 1;
		}
		row = mysql_fetch_row(res);
		mysql_free_result(res);
		length = strlen(row[0]);
		memcpy(friendIP, row[0], length);
		friendIP[length] = '\0';
		// send msg to friend
		friendAddr.sin_addr.s_addr = inet_addr(friendIP);
    		friendAddr.sin_port = htons((unsigned short)atoi(row[1]));
		if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			sprintf(sql, "%s%d,%d,'%s')", "insert into usr_resend_info(msg_recv_time,usr_id,usr_friend_id,usr_resend_msg) values (now(),", usrID, friendID, data);
			sql[93 + ((int)buffer[1]) + ((int)buffer[2]) + strlen(data)] = '\0';
			if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
			{
				sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
				ERROR_LOG(fp, tv, tim, "p2pChat, friend is online, but mysql is unreachable when put msg into usr_resend_info table\n");
				return 1;
			}
			sendFlagMsg(sock, SVR_SEND_MSG_FAIL_P2P);
			ERROR_LOG(fp, tv, tim, "p2pChat create socket failed, usrID=%d, friendID=%d\n", usrID, friendID);
			return 1;
		}
		if (connect(sfd, (struct sockaddr *)&friendAddr, sizeof(struct sockaddr)) < 0)
		{
			close(sfd);
			sprintf(sql, "%s%d,%d,'%s')", "insert into usr_resend_info(msg_recv_time,usr_id,usr_friend_id,usr_resend_msg) values (now(),", usrID, friendID, data);
			sql[93 + ((int)buffer[1]) + ((int)buffer[2]) + strlen(data)] = '\0';
			if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
			{
				sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
				ERROR_LOG(fp, tv, tim, "p2pChat, friend is online, but mysql is unreachable when put msg into usr_resend_info table\n");
				return 1;
			}
			sendFlagMsg(sock, SVR_SEND_MSG_FAIL_P2P);
			ERROR_LOG(fp, tv, tim, "p2pChat socket connect failed, usrID=%d, friendID=%d, friendIP=%s, friendPort=%s\n", usrID, friendID, friendIP, row[1]);
			return 1;
		}
		if (send(sfd, data, strlen(data), 0) < (int)strlen(data))
		{
			close(sfd);
			sprintf(sql, "%s%d,%d,'%s')", "insert into usr_resend_info(msg_recv_time,usr_id,usr_friend_id,usr_resend_msg) values (now(),", usrID, friendID, data);
			sql[93 + ((int)buffer[1]) + ((int)buffer[2]) + strlen(data)] = '\0';
			if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
			{
				sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
				ERROR_LOG(fp, tv, tim, "p2pChat, friend is online, but mysql is unreachable when put msg into usr_resend_info table\n");
				return 1;
			}
			sendFlagMsg(sock, SVR_SEND_MSG_FAIL_P2P);
			ERROR_LOG(fp, tv, tim, "p2pChat socket send failed, usrID=%d, friendID=%d, friendIP=%s, friendPort=%s\n", usrID, friendID, friendIP, row[1]);
			return 1;
		}		
		else
		{
			close(sfd);
		}		
	}
	// friend is offline, put msg into database resend table
	else
	{
		sprintf(sql, "%s%d,%d,'%s')", "insert into usr_resend_info(msg_recv_time,usr_id,usr_friend_id,usr_resend_msg) values (now(),", usrID, friendID, data);
		sql[93 + ((int)buffer[1]) + ((int)buffer[2]) + strlen(data)] = '\0';
		if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
		{
			sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
			ERROR_LOG(fp, tv, tim, "p2pChat, friend is offline, mysql is unreachable when put msg into usr_resend_info table\n");
			return 1;
		}
	}
	sendFlagMsg(sock, SEND_RECV_MSG_P2P);
	return 0;
}

int 
addGroupHandler 
(FILE * fp, char * dbIP, char * dbUserName, char * dbPass, char * dbName, int dbPort, int sock, char * buffer, char * data)
{
	struct timeval tv;
	struct tm tim;
	char sql[512];
	char temp[CHAR_MAX];
	int usrID;
	char groupName[CHAR_MAX];
	int groupID;
	int memNum;
	int onlineMemNum;
	int memID;
	int length1, length2, length3, length4;
	int groupIDLen, onlineMemNumLen;
	int i;
	char * index = NULL;

	//check if more than max groups
	sprintf(sql, "%s", "select count(1) from group_info");
	sql[31] = '\0';
	if (mysqlQueryCount(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql) >= MAX_GROUP_NUMBER)
	{
		sendFlagMsg(sock, SVR_RSP_REG_ERR_MAX_GRO);
		WARN_LOG(fp, tv, tim, "max group number is reached in this system, can not add group, current group number=%d\n", MAX_GROUP_NUMBER);
		return 1;
	}
	// get usrID
	length1 = (int)buffer[1];
	memcpy(temp, buffer + 2, length1);
	temp[length1] = '\0';
	usrID = atoi(temp);
	// get group name
	length2 = (int)buffer[(2 + length1)];
	memcpy(temp, buffer + 3 + length1, length2);
	temp[length2] = '\0';
	// check group name is exist or not
	sprintf(sql, "%s%s'", "select count(1) from group_info where group_name='", temp);
	sql[51 + length2] = '\0';
	if (0 != mysqlQueryCount(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
	{
		sendFlagMsg(sock, SVR_RSP_ADD_GRO_ERR_EXI);
		return 1;
	}
	memcpy(groupName, temp, length2);
	groupName[length2] = '\0';
	// get member number of this group
	length3 = (int)buffer[3 + length1 + length2];
	memcpy(temp, buffer + 4 + length1 + length2, length3);
	temp[length3] = '\0';
	memNum = atoi(temp);
	// write to group_info table of this group's basic info
	sprintf(sql, "%s%s',%d)", "insert into group_info(group_name,group_member_num) values('", groupName, memNum+1);
	// memNum = 9,99,999,...
	if (0 == ((memNum  + 1) % 10))
	{
		sql[63 + length2 +  length3 + 1] = '\0';
	}
	else
	{
		sql[63 + length2 +  length3] = '\0';
	}	
	if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
	{
		sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
		ERROR_LOG(fp, tv, tim, "addGroup, mysql is unreachable when update group_info table\n");
		return 1;
	}
	// get groupID from database
	sprintf(sql, "%s%s'", "select group_id from group_info where group_name='", groupName);
	sql[51 + length2] = '\0';
	if (0 != mysqlQuerySimple(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql, temp))
	{
		sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
		ERROR_LOG(fp, tv, tim, "addGroup, mysql is unreachable when get groupID from group_info table\n");
		return 1;
	}
	groupIDLen = strlen(temp);
	groupID = atoi(temp);
	// write into usr_group_info table
	sprintf(sql, "%s%d,%d)", "insert into usr_group_info(usr_id,group_id) values(", usrID, groupID);
	sql[53 + length1 +  groupIDLen] = '\0';
	if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
	{
		sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
		ERROR_LOG(fp, tv, tim, "addGroup, mysql is unreachable when insert into usr_group_info table\n");
		return 1;
	}
	// get member informations
	index = buffer + 4 + length1 + length2 + length3;
	for (i=0; i<memNum; i++)
	{
		length4 = (int)buffer[index - buffer];
		index ++;
		memcpy(temp, index, length4);
		temp[length4] = '\0';
		memID = atoi(temp);
		// update other members group info in usr_group_info table
		sprintf(sql, "%s%d,%d)", "insert into usr_group_info(usr_id,group_id) values(", memID, groupID);
		sql[53 + length4 +  groupIDLen] = '\0';
		if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
		{
			sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
			ERROR_LOG(fp, tv, tim, "addGroup, mysql is unreachable when update other members group info in usr_group_info table\n");
			return 1;
		}
		// update other members usr_group_num in usr_info table
		sprintf(sql, "%s%d", "update usr_info set usr_group_num=usr_group_num+1 where usr_id=", memID);
		sql[63 + length4] = '\0';
		if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
		{
			sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
			ERROR_LOG(fp, tv, tim, "addGroup, mysql is unreachable when update usr_group_num in usr_info table\n");
			return 1;
		}
		index += length4;
	}
	// get online member number of this group
	sprintf(sql, "%s%d", "select count(1) from usr_info i,usr_group_info g where i.usr_id=g.usr_id and i.usr_status='1' and g.group_id=", groupID);
	sql[109 + groupIDLen] = '\0';
	if (0 != mysqlQuerySimple(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql, temp))
	{
		sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
		ERROR_LOG(fp, tv, tim, "addGroup, mysql is unreachable when get online member number\n");
		return 1;
	}
	onlineMemNumLen = strlen(temp);
	onlineMemNum = atoi(temp);
	// update group_info table of online member number
	sprintf(sql, "%s%d%s%d)", "update group_info set group_online_member_num=", onlineMemNum, " where group_id=", groupID);
	sql[62 + onlineMemNumLen +  groupIDLen] = '\0';
	if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
	{
		sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
		ERROR_LOG(fp, tv, tim, "addGroup, mysql is unreachable when update group_info table of online member number\n");
		return 1;
	}
	// update adder self usr_group_num in usr_info table
	sprintf(sql, "%s%d", "update usr_info set usr_group_num=usr_group_num+1 where usr_id=", usrID);
	sql[63 + length1] = '\0';
	if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
	{
		sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
		ERROR_LOG(fp, tv, tim, "addGroup, mysql is unreachable when update usr_group_num in usr_info table\n");
		return 1;
	}
	data[0] = SVR_RSP_ADD_GRO_SUC;
	index = data + 1;
	data[index - data] = groupIDLen;
	index ++;
	sprintf(index, "%d", groupID);
	index += groupIDLen;
	data[index - data] = '\0';			
	send(sock, data, strlen(data), 0);
	INFO_LOG(fp, tv, tim, "add group successfully, usrID=%d, groupID=%d, groupName=%s, groupMemNum=%d\n",usrID, groupID, groupName, memNum+1);
	return 0;
}

int 
groupChatHandler 
(FILE * fp, char * dbIP, char * dbUserName, char * dbPass, char * dbName, int dbPort, int sock, char * buffer, char * data)
{
	struct timeval tv;
	struct tm tim;
	char sql[512];
	char temp[CHAR_MAX];
	int usrID;
	int groupID;
	char senderName[CHAR_MAX];
	char * index = NULL;
	int sfd = -1;
	struct sockaddr_in friendAddr;
	int friendID;
	char friendIP[16];
	MYSQL_RES * res = NULL;
	MYSQL_ROW row;
	int rowsNum;
	int i;	
	int length1, length2, length3, friendIDLen, senderNameLen;

	memset(&friendAddr, 0, sizeof(struct sockaddr_in));
    	friendAddr.sin_family = AF_INET;
	
	// get usrID
	length1 = (int)buffer[1];
	memcpy(temp, buffer + 2, length1);
	temp[length1] = '\0';
	usrID = atoi(temp);
	// get groupID
	length2 = (int)buffer[(2 + length1)];
	memcpy(temp, buffer + 3 + length1, length2);
	temp[length2] = '\0';
	groupID = atoi(temp);
	// query sender name from database
	sprintf(sql, "%s%d", "select usr_name from usr_info where usr_id=", usrID);
	sql[43 + length1] = '\0';
	if (0 != mysqlQuerySimple(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql, senderName))
	{
		sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
		ERROR_LOG(fp, tv, tim, "p2pChat, mysql is unreachable when query friend status from usr_info table\n");
		return 1;
	}
	senderNameLen = strlen(senderName);
	// make msg prepare to send to members in group
	data[0] = SEND_RECV_MSG_GRO;
	data[1] = buffer[(2 + length1)];
	index = data + 2;
	memcpy(index, buffer + 3 + length1, length2);
	index += length2;
	data[index - data] = senderNameLen;
	index ++;
	memcpy(index, senderName, senderNameLen);
	index += senderNameLen;
	length3 = (int)buffer[(3 + length1 + length2)];
	data[index - data] = buffer[(3 + length1 + length2)];
	index ++;
	memcpy(index, buffer + 4 + length1 + length2, length3);
	index += length3;
	data[index - data] = '\0';
	// query online members ip and port from database, send msg to them
	sprintf(sql, "%s%d%s%d", "select i.usr_id,usr_local_ip,usr_local_port from usr_group_info g,usr_info i where g.usr_id=i.usr_id and i.usr_status='1' and g.group_id=", groupID, " and not g.usr_id=", usrID);
	sql[155 + length1 + length2] = '\0';
	res = mysqlQueryGetRes(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql);
	if ( ! res )
	{
		// all members of this group are offline
		sprintf(sql, "%s%d%s%d", "select usr_id from usr_group_info g where g.group_id=", groupID, " and not g.usr_id=", usrID);
		sql[71 + length1 + length2] = '\0';
		res = mysqlQueryGetRes(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql);
		if ( ! res )
		{
			ERROR_LOG(fp, tv, tim, "internal error, groupChat query all offline member id from database return empty, sql=%s\n", sql);
			return 1;
		}
		rowsNum = mysql_num_rows(res);
		for (i=0; i<rowsNum; i++)
		{
			row = mysql_fetch_row(res);
			friendID = atoi(row[0]);
			friendIDLen = strlen(row[0]);
			sprintf(sql, "%s%d,%d,'%s')", "insert into usr_resend_info(msg_recv_time,usr_id,usr_friend_id,usr_resend_msg) values (now(),", usrID, friendID, data);
			sql[93 + ((int)buffer[1]) + ((int)buffer[2]) + strlen(data)] = '\0';
			if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
			{
				sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
				ERROR_LOG(fp, tv, tim, "groupChat, friend is offline, but mysql is unreachable when put msg into usr_resend_info table\n");
				return 1;
			}
		}
		mysql_free_result(res);
		return 0;
	}
	rowsNum = mysql_num_rows(res);
	for (i=0; i<rowsNum; i++)
	{
		row = mysql_fetch_row(res);
		friendID = atoi(row[0]);
		friendIDLen = strlen(row[0]);
		memcpy(friendIP, row[1], strlen(row[1]));
		friendIP[strlen(friendIP)] = '\0';
		// send msg to friend
		friendAddr.sin_addr.s_addr = inet_addr(friendIP);
    		friendAddr.sin_port = htons((unsigned short)atoi(row[2]));
		if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			sprintf(sql, "%s%d,%d,'%s')", "insert into usr_resend_info(msg_recv_time,usr_id,usr_friend_id,usr_resend_msg) values (now(),", usrID, friendID, data);
			sql[93 + ((int)buffer[1]) + ((int)buffer[2]) + strlen(data)] = '\0';
			if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
			{
				sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
				ERROR_LOG(fp, tv, tim, "groupChat, friend is online, but mysql is unreachable when put msg into usr_resend_info table\n");
				return 1;
			}
			sendFlagMsg(sock, SVR_SEND_MSG_FAIL_P2P);
			ERROR_LOG(fp, tv, tim, "groupChat create socket failed, usrID=%d, friendID=%d\n", usrID, friendID);
			return 1;
		}
		if (connect(sfd, (struct sockaddr *)&friendAddr, sizeof(struct sockaddr)) < 0)
		{
			close(sfd);
			sprintf(sql, "%s%d,%d,'%s')", "insert into usr_resend_info(msg_recv_time,usr_id,usr_friend_id,usr_resend_msg) values (now(),", usrID, friendID, data);
			sql[93 + ((int)buffer[1]) + ((int)buffer[2]) + strlen(data)] = '\0';
			if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
			{
				sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
				ERROR_LOG(fp, tv, tim, "groupChat, friend is online, but mysql is unreachable when put msg into usr_resend_info table\n");
				return 1;
			}
			sendFlagMsg(sock, SVR_SEND_MSG_FAIL_P2P);
			ERROR_LOG(fp, tv, tim, "groupChat socket connect failed, usrID=%d, friendID=%d, friendIP=%s, friendPort=%s\n", usrID, friendID, friendIP, row[2]);
			return 1;
		}
		if (send(sfd, data, strlen(data), 0) < (int)strlen(data))
		{
			close(sfd);
			sprintf(sql, "%s%d,%d,'%s')", "insert into usr_resend_info(msg_recv_time,usr_id,usr_friend_id,usr_resend_msg) values (now(),", usrID, friendID, data);
			sql[93 + ((int)buffer[1]) + ((int)buffer[2]) + strlen(data)] = '\0';
			if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
			{
				sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
				ERROR_LOG(fp, tv, tim, "groupChat, friend is online, but mysql is unreachable when put msg into usr_resend_info table\n");
				return 1;
			}
			sendFlagMsg(sock, SVR_SEND_MSG_FAIL_P2P);
			ERROR_LOG(fp, tv, tim, "groupChat socket send failed, usrID=%d, friendID=%d, friendIP=%s, friendPort=%s\n", usrID, friendID, friendIP, row[2]);
			return 1;
		}		
		else
		{
			close(sfd);
		}
	}
	mysql_free_result(res);
	// query offline members id from database, put msg into resend table
	sprintf(sql, "%s%d%s%d", "select i.usr_id from usr_group_info g,usr_info i where g.usr_id=i.usr_id and i.usr_status='0' and g.group_id=", groupID, " and not g.usr_id=", usrID);
	sql[127 + length1 + length2] = '\0';
	res = mysqlQueryGetRes(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql);
	if ( ! res )
	{
		// all members of this group are online
		return 0;
	}
	rowsNum = mysql_num_rows(res);
	for (i=0; i<rowsNum; i++)
	{
		row = mysql_fetch_row(res);
		friendID = atoi(row[0]);
		friendIDLen = strlen(row[0]);
		sprintf(sql, "%s%d,%d,'%s')", "insert into usr_resend_info(msg_recv_time,usr_id,usr_friend_id,usr_resend_msg) values (now(),", usrID, friendID, data);
		sql[93 + ((int)buffer[1]) + ((int)buffer[2]) + strlen(data)] = '\0';
		if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
		{
			sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
			ERROR_LOG(fp, tv, tim, "groupChat, friend is offline, but mysql is unreachable when put msg into usr_resend_info table\n");
			return 1;
		}
	}
	mysql_free_result(res);
	sendFlagMsg(sock, SEND_RECV_MSG_GRO);
	return 0;
}

int 
logoutHandler 
(FILE * fp, char * dbIP, char * dbUserName, char * dbPass, char * dbName, int dbPort, int sock, char * buffer)
{
	struct timeval tv;
	struct tm tim;
	char sql[512];
	char temp[CHAR_MAX];
	int usrID;

	// get usrID
	memcpy(temp, buffer + 2, (int)buffer[1]);
	temp[((int)buffer[1])] = '\0';
	usrID = atoi(temp);
	// check usr if is already logout
	sprintf(sql, "%s%d%s", "select count(1) from usr_info where usr_id=", usrID, " and usr_status='0'");
	sql[62 + (int)*(buffer + 1)] = '\0';
	if (0 != mysqlQueryCount(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
	{
		sendFlagMsg(sock, SVR_RSP_USR_LOG_OUT_REP);
		return 1;
	}
	// update usr_info table
	sprintf(sql, "%s%c%s%d", "update usr_info set usr_status=", USER_STATUS_OFFLINE, 
		",usr_local_ip='',usr_local_port=0 where usr_id=", usrID);
	sql[79 + ((int)buffer[1])] = '\0';
	if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
	{
		sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
		ERROR_LOG(fp, tv, tim, "logout, mysql is unreachable when update usr_info table\n");
		return 1;
	}
	// update group_online_member_num in group_info table
	sprintf(sql, "%s%d%s", "update group_info g,usr_group_info u set g.group_online_member_num=g.group_online_member_num-1 where u.usr_id=", usrID, " and u.group_id=g.group_id");
	sql[136 + ((int)buffer[1])] = '\0';
	if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
	{
		sendFlagMsg(sock, SVR_MYSQL_UNREACHABLE);
		ERROR_LOG(fp, tv, tim, "logout, mysql is unreachable when update group_online_member_num in group_info table\n");
		return 1;
	}
	// logout success response
	sendFlagMsg(sock, SVR_RSP_LON_OUT_SUC);
	INFO_LOG(fp, tv, tim, "usr logout successfully, usrID=%d\n",usrID);
	return 0;
}

int 
queryGroupMemberListHandler 
(FILE * fp, char * dbIP, char * dbUserName, char * dbPass, char * dbName, int dbPort, int sock, char * buffer, char * data)
{
	struct timeval tv;
	struct tm tim;
	char sql[512];
	char temp[CHAR_MAX];
	int usrID;
	int groupID;
	int length1, length2, length3, length4;
	int rowsNum;
	int i;
	char * index = NULL;
	MYSQL_RES * res = NULL;
	MYSQL_ROW row;

	// get usrID
	length1 = (int)buffer[1];
	length2 = (int)buffer[2];
	memcpy(temp, buffer + 3, length1);
	temp[length1] = '\0';
	usrID = atoi(temp);
	// get groupID
	memcpy(temp, buffer + 3 + length1, length2);
	temp[length2] = '\0';
	groupID = atoi(temp);
	// query member list of this group
	sprintf(sql, "%s%d", "select usr_name from usr_info i,usr_group_info g where i.usr_id=g.usr_id and g.group_id=", groupID);
	sql[88 + length1 + length2] = '\0';
	res = mysqlQueryGetRes(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql);
	if ( ! res )
	{
		sendFlagMsg(sock, GRO_MEM_LST_REQ_ERR_RSP);
		ERROR_LOG(fp, tv, tim, "internal error, query member list of usr's group return empty, sql=%s\n", sql);
		return 1;
	}
	data[0] = GRO_MEM_LST_REQ_SUC_RSP;
	rowsNum = mysql_num_rows(res);
	snprintf(temp, CHAR_MAX, "%d", rowsNum);
	length3 = strlen(temp);
	data[1] = length3;
	index = data + 2;
	memcpy(index, temp, length3);
	index += length3;
	for (i=0; i<rowsNum; i++)
	{
		row = mysql_fetch_row(res);
		length4 = strlen(row[0]);
		data[index - data] = length4;
		index ++;
		memcpy(index, row[0], length4);
		index += length4;
	}
	data[index - data] = '\0';
	send(sock, data, strlen(data), 0);
	return 0;
}

void 
heartbeatHandler 
(FILE * fp, char * dbIP, char * dbUserName, char * dbPass, char * dbName, int dbPort)
{
	struct timeval tv;
	struct tm tim;
	char sql[512];
	int rowsNum;
	int i;
	int usrID;
	struct sockaddr_in addr;
	int sfd = -1;
	MYSQL_RES * res = NULL;
	MYSQL_ROW row;

	memset(&addr, 0, sizeof(struct sockaddr_in));
    	addr.sin_family = AF_INET;

	for (;;)
	{
		// query all online usr's network info
		memcpy(sql, "select usr_id,usr_local_ip,usr_local_port from usr_info where usr_status='1'", 76);
		sql[76] = '\0';
		res = mysqlQueryGetRes(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql);
		if ( ! res )
		{
			ERROR_LOG(fp, tv, tim, "internal error, query all online usr's network info return empty, sql=%s\n", sql);
			sleep(10);
			continue;
		}
		rowsNum = mysql_num_rows(res);
		if (0 == rowsNum)
		{
			sleep(10);
			continue;
		}
		// check each usr's heartbeat is alive or not
		for (i=0; i<rowsNum; i++)
		{
			row = mysql_fetch_row(res);
			usrID = atoi(row[0]);
			addr.sin_addr.s_addr = inet_addr(row[1]);
    			addr.sin_port = htons((unsigned short)atoi(row[2]));
			if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
				continue;
			if (connect(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) < 0)
			{
				close(sfd);
				markUsrOnline2Offline(fp,dbIP, dbUserName, dbPass, dbName, dbPort, usrID);
				WARN_LOG(fp, tv, tim, "usr seems offline, mark him/her offline force, usrID=[%d]\n", usrID);
				continue;
			}
			if (0 != sendRecvHeartbeat(sfd))
			{
				markUsrOnline2Offline(fp,dbIP, dbUserName, dbPass, dbName, dbPort, usrID);
				WARN_LOG(fp, tv, tim, "usr seems offline, mark him/her offline force, usrID=[%d]\n", usrID);
				continue;
			}
		}
		sleep(HEARTBEAT_INTERVAL);
	}
}

void 
markUsrOnline2Offline 
(FILE * fp, char * dbIP, char * dbUserName, char * dbPass, char * dbName, int dbPort, int usrID)
{
	struct timeval tv;
	struct tm tim;
	char sql[512];
	char temp[CHAR_MAX];

	snprintf(temp, CHAR_MAX, "%d", usrID);
	int usrIdLen = strlen(temp);

	// check if this usr is really online
	sprintf(sql, "%s%d%s", "select count(1) from usr_info where usr_id=", usrID, " and usr_status='1'");
	sql[62 + usrIdLen] = '\0';
	// online, update database
	if (0 != mysqlQueryCount(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
	{
		// update usr_info
		sprintf(sql, "%s%c%s%d", "update usr_info set usr_status=", USER_STATUS_OFFLINE, ",usr_local_ip='',usr_local_port=0 where usr_id=", usrID);
		sql[79 + usrIdLen] = '\0';
		if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
		{
			ERROR_LOG(fp, tv, tim, "markUsrOnline2Offline, mysql is unreachable when update usr_info table\n");
			return;
		}
		// update group_info
		sprintf(sql, "%s%d", "update group_info g,usr_group_info u set g.group_online_member_num=g.group_online_member_num-1 where u.group_id=g.group_id and u.usr_id=", usrID);
		sql[136 + usrIdLen] = '\0';
		if (0 != mysqlInsUpdDel(fp, dbIP, dbUserName, dbPass, dbName, dbPort, sql))
		{
			ERROR_LOG(fp, tv, tim, "markUsrOnline2Offline, mysql is unreachable when update group_info table\n");
			return;
		}
		return;
	}
	// offline, do nothing
	else
	{
		return;
	}
}


// mysql db
#define MYSQL_USRNAME				"chat"
#define MYSQL_USRPASS				"chat"
#define MYSQL_DBNAME				"chatDB"
#define MYSQL_DBPORT				3306

// heartbeat interval (seconds)
#define HEARTBEAT_INTERVAL			30

#define MAX_USER_NUMBER 			CHAR_MAX
#define MAX_GROUP_NUMBER			CHAR_MAX
#define MAX_STORE_RESEND_MSG		8192

// login success
#define SVR_RSP_LON_IN_SUC 			'\x01'	// 0000 0001
// register success
#define SVR_RSP_REG_SUC 				'\x02'	// 0000 0010
// add friend success
#define SVR_RSP_ADD_FRI_SUC			'\x03'	// 0000 0011
// send and recv msg by p2p
#define SEND_RECV_MSG_P2P			'\x04'	// 0000 0100
// add group success
#define SVR_RSP_ADD_GRO_SUC			'\x05'	// 0000 0101
// send and recv msg by group
#define SEND_RECV_MSG_GRO			'\x06'	// 0000 0110
// logout success
#define SVR_RSP_LON_OUT_SUC			'\x07'	// 0000 0111
// query group member list
#define GRO_MEM_LST_REQ_SUC_RSP		'\x08'	// 0000 1000
// heartbeat between server and client
#define HEART_BEAT_REQ_RSP			'\x09'	// 0000 1001
//mysql is unreachable
#define SVR_MYSQL_UNREACHABLE		'\x80'	// 1000 0000
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
// server send msg to friend failed
#define SVR_SEND_MSG_FAIL_P2P		'\x87'	// 1000 0111
// add group more than max group number
#define SVR_RSP_REG_ERR_MAX_GRO		'\x88'	// 1000 1000
// group is already exist
#define SVR_RSP_ADD_GRO_ERR_EXI		'\x89'	// 1000 1001
// usr is already login
#define SVR_RSP_USR_LOG_IN_REP		'\x8A'	// 1000 1010
// usr is already logout
#define SVR_RSP_USR_LOG_OUT_REP		'\x8B'	// 1000 1011
// query group member list error
#define GRO_MEM_LST_REQ_ERR_RSP		'\x8C'	// 1000 1100

#define USER_STATUS_ONLINE			'1'
#define USER_STATUS_OFFLINE			'0'

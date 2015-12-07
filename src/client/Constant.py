# encoding=utf-8

# friend status:
FRI_ONLINE = '1'
FRI_OFFLINE = '0'

# mysql db is not reachable
SVR_MYSQL_UNREACHABLE = '\x80'

# login:
# client login and server response success both use 0000 0001(\x01)
LON_IN_REQ_SUC_RSP = '\x01'
# server response when client authentication failed, 1000 0011(\x83)
SVR_RSP_LON_ERR_REP = '\x83'
# server response when user is not in memory database, 1000 0100(\x84)
SVR_RSP_LON_ERR_NOT_EXI = '\x84'
# server response when user is already login, 1000 1010(\x8A)
SVR_RSP_USR_LOG_IN_REP = '\x8A'

# register:
# client register and server response success both use 0000 0010(\x02)
REG_REQ_SUC_RSP = '\x02'
# server response when client reach max user number, 1000 0001(\x81)
SVR_RSP_REG_ERR_MAX_USR = '\x81'
# server response when client use same name register twice, 1000 0010(\x82)
SVR_RSP_REG_ERR_REP = '\x82'

# add friend:
# client add friend and server response success both use 0000 0011(\x03)
ADD_FRI_REQ_SUC_RSP = '\x03'
# server response when client add friend but not exist, 1000 0101(\x85)
SVR_RSP_ADD_FRI_ERR_NOT_EXI = '\x85'
# server response when client add friend but already added, 1000 0110(\x86)
SVR_RSP_ADD_FRI_ERR_AGN = '\x86'

# send and recv msg p2p:
# p2p client send msg and server send to other success both use 0000 0100(\x04)
SEND_RECV_REQ_SUC_RSP_P2P = '\x04'
# p2p server send msg to friend(s) failed, 1000 0111(\x87)
SVR_SEND_MSG_FAIL_P2P = '\x87'

# add group:
# client add group and server response success both use 0000 0101(\x05)
ADD_GRO_REQ_SUC_RSP = '\x05'
# server response when client add group reach max group number, 1000 1000(\x88)
SVR_RSP_REG_ERR_MAX_GRO = '\x88'
# server response when client add group but already exist, 1000 1001(\x89)
SVR_RSP_ADD_GRO_ERR_EXI = '\x89'

# send and recv msg group:
# group client send msg and server send to other success both use 0000 0110(\x06)
SEND_RECV_REQ_SUC_RSP_GRO = '\x06'

# logout:
# client logout and server response success both use 0000 0111(\x07)
LON_OUT_REQ_SUC_RSP = '\x07'
# server response when user is already logout, 1000 1011(\x8B)
SVR_RSP_USR_LOG_OUT_REP = '\x8B'

# query group member list request:
# client query group member list and server response success both use 0000 1000(\x08)
GRO_MEM_LST_REQ_SUC_RSP = '\x08'
# query group member list error, 1000 1100(\x8C)
GRO_MEM_LST_REQ_ERR_RSP = '\x8C'

# heart between client and server, use 0000 1001(\x09)
CLI_SVR_HEART_BEAT = '\x09'

#! /usr/bin/python
# encoding=utf-8

# login:
# client login and server response success both use 0000 0001(\x01)
LON_REQ_SUC_RSP = '\x01'
# server response when client authentication failed, 1000 0011(\x83)
SVR_RSP_LON_ERR_REP = '\x83'
# server response when user is not in memory database, 1000 0100(\x84)
SVR_RSP_LON_ERR_NOT_EXI = '\x84'

# register:
# client register and server response success both use 0000 0010(\x02)
REG_REQ_SUC_RSP = '\x02'
# server response when client reach max user number, 1000 0001(\x81)
SVR_RSP_REG_ERR_MAX_USR = '\x81'
# server response when client use same name register twice, 1000 0010(\x82)
SVR_RSP_REG_ERR_REP = '\x82'

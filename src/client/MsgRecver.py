#! /usr/bin/python
# encoding=utf-8

import threading
import socket
import time
import Login
import Constant
import Tools
import P2P_Chat

SENDER_ID = ""
SENDER_MSG = ""
LOCAL_IP = ""

class MsgRecv(threading.Thread):

    def run(self):

        global SENDER_ID
        global SENDER_MSG
        global LOCAL_IP

        LOCAL_IP = Tools.get_local_ip()
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        while True:
            if not Login.isParamOK:
                time.sleep(1)
            else:
                break

        sock.bind((LOCAL_IP, int(Login.LOCAL_PORT)))
        sock.listen(64)

        while True:
            connection, address = sock.accept()
            try:
                connection.settimeout(10)
                buf = connection.recv(1024)
                if Constant.SEND_RECV_REQ_SUC_RSP == buf[0]:
                    senderID_len = ord(buf[1])
                    msg_len = ord(buf[2])
                    index = 3
                    SENDER_ID = buf[index:(index + senderID_len)]
                    index += senderID_len
                    SENDER_MSG = buf[index:(index + msg_len)]
                    P2P_Chat.MSG_RECV.AppendText(SENDER_MSG + '\n')
                    P2P_Chat.MSG_RECV.Update()
                else:
                    connection.close()
                    continue
            except Exception, e:
                continue
            finally:
                connection.close()

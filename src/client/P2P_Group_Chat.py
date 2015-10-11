#! /usr/bin/python
# encoding=utf-8

import wx
import socket
import threading
import time
import FriendList
import Constant
import Login
import Dialog
import Tools

MSG_RECV = None
LOCAL_IP = ""

class MsgRecv(threading.Thread):

    def run(self):

        global SENDER_ID
        global SENDER_MSG
        global LOCAL_IP
        global MSG_RECV

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
                if Constant.SEND_RECV_REQ_SUC_RSP_P2P == buf[0]:
                    senderID_len = ord(buf[1])
                    msg_len = ord(buf[2])
                    index = 3
                    SENDER_ID = buf[index:(index + senderID_len)]
                    index += senderID_len
                    SENDER_MSG = buf[index:(index + msg_len)]
                    MSG_RECV.AppendText(FriendList.P2P_NAME + "  "
                                        + time.strftime('%Y-%m-%d %X', time.localtime(time.time())) + "\n"
                                        + str(SENDER_MSG).decode('utf-8') + '\n')
                else:
                    connection.close()
                    continue
            except Exception, e:
                continue
            finally:
                connection.close()


class P2P_Chat(wx.Frame):

    msg_recv = None
    msg_send = None
    isSuccess = True

    def __init__(self, parent):

        global MSG_RECV

        wx.Frame.__init__(self, parent,
                          title=u'%s' % FriendList.P2P_NAME,
                          size=(500, 500),
                          style=wx.MINIMIZE_BOX
                                | wx.RESIZE_BORDER
                                | wx.SYSTEM_MENU
                                | wx.CAPTION
                                | wx.CLOSE_BOX
                                | wx.CLIP_CHILDREN)
        p2p_panel = wx.Panel(self)
        MSG_RECV = wx.TextCtrl(p2p_panel, size=(450, 300), style=wx.TE_READONLY | wx.TE_MULTILINE)
        MSG_RECV.SetBackgroundColour(wx.Colour(192, 220, 192))
        self.msg_send = wx.TextCtrl(p2p_panel, size=(450, 100), style=wx.TE_PROCESS_ENTER)
        self.msg_send.SetBackgroundColour(wx.Colour(192, 220, 192))
        vbox = wx.BoxSizer(wx.VERTICAL)
        vbox.Add(MSG_RECV,
                 proportion=0,
                 flag=wx.UP | wx.LEFT | wx.RIGHT | wx.EXPAND,
                 border=5)
        vbox.Add((-1, 20))
        vbox.Add(self.msg_send,
                 proportion=1,
                 flag=wx.Bottom | wx.LEFT | wx.RIGHT | wx.EXPAND,
                 border=5)
        p2p_panel.SetSizer(vbox)

        self.msg_send.Bind(wx.EVT_TEXT_ENTER, self.On_Send_Clear, self.msg_send)
        pass

    def On_Send_Clear(self, event):

        global MSG_RECV

        rsp = None
        msg = self.msg_send.GetValue().encode('utf-8')
        send_data = Constant.SEND_RECV_REQ_SUC_RSP_P2P + \
            chr(len(str(Login.USERID))) + \
            chr(len(str(FriendList.P2P_ID))) + \
            chr(len(msg)) + \
            str(Login.USERID) + \
            str(FriendList.P2P_ID) + \
            msg
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect((Login.SERVER_IP, int(Login.SERVER_PORT)))
            sock.send(send_data)
            rsp = sock.recv(8192)
        except Exception, e:
            self.isSuccess = False
            dialog = Dialog.Dialog(None, u"错误", e.message, 200, 150, 30, 60)
            dialog.Centre()
            dialog.Show()
        finally:
            sock.close()

        # 消息已经成功发往服务器
        if self.isSuccess:
            self.msg_send.Clear()
            MSG_RECV.AppendText(Login.USERNAME + "  "
                                + time.strftime('%Y-%m-%d %X', time.localtime(time.time())) + "\n"
                                + str(msg).decode('utf-8') + "\n")
        else:
            dialog = Dialog.Dialog(None, u"错误", u"消息发送失败", 200, 150, 30, 60)
            dialog.Centre()
            dialog.Show()


class Panel_Single(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        pass

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

MSG_RECV_P2P = None
MSG_RECV_GROUP = None
LOCAL_IP = ""


class MsgRecv(threading.Thread):
    def run(self):

        global SENDER_ID
        global SENDER_MSG
        global LOCAL_IP
        global MSG_RECV_P2P
        global MSG_RECV_GROUP

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
                buf = connection.recv(8192)
                if Constant.SEND_RECV_REQ_SUC_RSP_P2P == buf[0]:
                    senderID_len = ord(buf[1])
                    msg_len = ord(buf[2])
                    index = 3
                    SENDER_ID = buf[index:(index + senderID_len)]
                    index += senderID_len
                    SENDER_MSG = buf[index:(index + msg_len)]
                    if MSG_RECV_P2P is not None:
                        MSG_RECV_P2P.AppendText(FriendList.P2P_NAME + "  "
                                                + time.strftime('%Y-%m-%d %X', time.localtime(time.time())) + "\n"
                                                + str(SENDER_MSG).decode('utf-8'))
                    else:
                        pass
                elif Constant.SEND_RECV_REQ_SUC_RSP_GRO == buf[0]:
                    groupID_len = ord(buf[1])
                    index = 2
                    groupID = buf[index:(index + groupID_len)]
                    index += groupID_len
                    senderName_len = ord(buf[index])
                    index += 1
                    SENDER_Name = buf[index:(index + senderName_len)]
                    index += senderName_len
                    msg_len = ord(buf[index])
                    index += 1
                    SENDER_MSG = buf[index:(index + msg_len)]
                    MSG_RECV_GROUP.AppendText(str(SENDER_Name) + "  "
                                              + time.strftime('%Y-%m-%d %X', time.localtime(time.time())) + "\n"
                                              + str(SENDER_MSG).decode('utf-8'))
                # heartbeat reply
                elif Constant.CLI_SVR_HEART_BEAT == buf[0]:
                    try:
                        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                        sock.connect((Login.SERVER_IP, int(Login.SERVER_PORT)))
                        sock.send(Constant.CLI_SVR_HEART_BEAT)
                    except Exception, e:
                        dialog = Dialog.Dialog(None, u"错误", e.message, 200, 150, 30, 60)
                        dialog.Centre()
                        dialog.Show()
                    finally:
                        sock.close()
                else:
                    connection.close()
                    continue
            except Exception, e:
                continue
            finally:
                connection.close()


class P2P_Chat(wx.Frame):
    msg_send = None
    isSuccess = True

    def __init__(self, parent, p2pName):

        global MSG_RECV_P2P

        wx.Frame.__init__(self, parent,
                          title=u'%s' % p2pName,
                          size=(500, 500),
                          style=wx.MINIMIZE_BOX
                                | wx.RESIZE_BORDER
                                | wx.SYSTEM_MENU
                                | wx.CAPTION
                                | wx.CLOSE_BOX
                                | wx.CLIP_CHILDREN)
        p2p_panel = wx.Panel(self)
        MSG_RECV_P2P = wx.TextCtrl(p2p_panel, size=(450, 300), style=wx.TE_READONLY | wx.TE_MULTILINE)
        self.msg_send = wx.TextCtrl(p2p_panel,
                                    size=(450, 100),
                                    style=wx.TE_RICH2 | wx.TE_MULTILINE | wx.TE_PROCESS_ENTER)
        # 使初始输入光标出现在文本输入框
        self.msg_send.SetFocus()
        vbox = wx.BoxSizer(wx.VERTICAL)
        vbox.Add(MSG_RECV_P2P,
                 proportion=0,
                 flag=wx.UP | wx.LEFT | wx.RIGHT | wx.EXPAND,
                 border=5)
        vbox.Add((-1, 10))
        vbox.Add(self.msg_send,
                 proportion=1,
                 flag=wx.Bottom | wx.LEFT | wx.RIGHT | wx.EXPAND,
                 border=5)
        p2p_panel.SetSizer(vbox)

        self.msg_send.Bind(wx.EVT_TEXT_ENTER, self.On_Send_Clear, self.msg_send)
        pass

    def On_Send_Clear(self, event):

        global MSG_RECV_P2P

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
            rsp = sock.recv(16)
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
            MSG_RECV_P2P.AppendText(Login.USERNAME + "  "
                                    + time.strftime('%Y-%m-%d %X', time.localtime(time.time())) + "\n"
                                    + str(msg).decode('utf-8'))
        else:
            dialog = Dialog.Dialog(None, u"错误", u"消息发送失败", 200, 150, 30, 60)
            dialog.Centre()
            dialog.Show()


class Group_Chat(wx.Frame):
    msg_send = None
    group_mem_list = None
    isSuccess = True

    def __init__(self, parent, groupName):

        global MSG_RECV_GROUP

        wx.Frame.__init__(self, parent,
                          title=u'%s' % groupName,
                          size=(550, 500),
                          style=wx.MINIMIZE_BOX
                                | wx.RESIZE_BORDER
                                | wx.SYSTEM_MENU
                                | wx.CAPTION
                                | wx.CLOSE_BOX
                                | wx.CLIP_CHILDREN)
        group_panel = wx.Panel(self)
        MSG_RECV_GROUP = wx.TextCtrl(group_panel, size=(400, 150), style=wx.TE_READONLY | wx.TE_MULTILINE)
        self.msg_send = wx.TextCtrl(group_panel,
                                    size=(400, 150),
                                    style=wx.TE_RICH2 | wx.TE_MULTILINE | wx.TE_PROCESS_ENTER)
        self.group_mem_list = wx.TreeCtrl(group_panel, size=(120, 300), style=wx.TR_HIDE_ROOT | wx.TR_HAS_BUTTONS)
        root = self.group_mem_list.AddRoot('root')
        gName = self.group_mem_list.AppendItem(root, u'群成员')
        # 逐个填充群聊右侧窗口中的群组成员的名字
        for i in range(0, FriendList.GROUP_MEM_NUM, 1):
            self.group_mem_list.AppendItem(gName, FriendList.GROUP_MEM_NAME_ARRAY[i])
        self.group_mem_list.ExpandAll()

        # 使初始输入光标出现在文本输入框
        self.msg_send.SetFocus()
        vbox = wx.BoxSizer(wx.VERTICAL)
        hbox = wx.BoxSizer(wx.HORIZONTAL)
        vbox.Add(MSG_RECV_GROUP,
                 proportion=1,
                 flag=wx.TOP | wx.LEFT,
                 border=5)
        vbox.Add((-1, 10))
        vbox.Add(self.msg_send,
                 proportion=0,
                 flag=wx.LEFT,
                 border=5)

        hbox.Add(vbox,
                 proportion=1,
                 flag=wx.LEFT | wx.EXPAND,
                 border=5)
        hbox.Add((-1, 20))
        hbox.Add(self.group_mem_list,
                 proportion=0,
                 flag=wx.RIGHT | wx.EXPAND,
                 border=5)
        group_panel.SetSizer(hbox)

        self.msg_send.Bind(wx.EVT_TEXT_ENTER, self.On_Send_Clear, self.msg_send)
        pass

    def On_Send_Clear(self, event):
        global MSG_RECV_GROUP
        rsp = None
        msg = self.msg_send.GetValue().encode('utf-8')
        send_data = Constant.SEND_RECV_REQ_SUC_RSP_GRO + \
                    chr(len(str(Login.USERID))) + \
                    str(Login.USERID) + \
                    chr(len(str(FriendList.GROUP_ID))) + \
                    str(FriendList.GROUP_ID) + \
                    chr(len(msg)) + \
                    msg
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect((Login.SERVER_IP, int(Login.SERVER_PORT)))
            sock.send(send_data)
            rsp = sock.recv(16)
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
            MSG_RECV_GROUP.AppendText(Login.USERNAME + "  "
                                      + time.strftime('%Y-%m-%d %X', time.localtime(time.time())) + "\n"
                                      + str(msg).decode('utf-8'))
        else:
            dialog = Dialog.Dialog(None, u"错误", u"消息发送失败", 200, 150, 30, 60)
            dialog.Centre()
            dialog.Show()

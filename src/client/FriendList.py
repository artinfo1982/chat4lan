# encoding=utf-8

import wx
import os
import socket
import Dialog
import AddFriend
import AddGroup
import Login
import Constant
import P2P_Group_Chat

P2P_ID = ""
P2P_NAME = ""
GROUP_ID = ""
GROUP_NAME = ""
GROUP_MEM_NUM = 0
GROUP_MEM_NAME_ARRAY = {}

class FriendList(wx.Frame):
    def __init__(self, parent):
        wx.Frame.__init__(self, parent,
                          title=u'局域网聊天',
                          size=(270, 500),
                          style=wx.MINIMIZE_BOX
                                | wx.RESIZE_BORDER
                                | wx.SYSTEM_MENU
                                | wx.CAPTION
                                | wx.CLOSE_BOX
                                | wx.CLIP_CHILDREN)
        notebook = wx.Notebook(self)
        notebook.AddPage(Panel_Single(notebook), u"好友列表")
        notebook.AddPage(Panel_Multi(notebook), u"群组")

        # 绑定关闭事件
        self.Bind(wx.EVT_CLOSE, self.On_Close)

    def On_Close(self, event):
        send_data = Constant.LON_OUT_REQ_SUC_RSP + \
            chr(len(str(Login.USERID))) + \
            str(Login.USERID)
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(10)
            sock.connect((Login.SERVER_IP, int(Login.SERVER_PORT)))
            sock.send(send_data)
            rsp = sock.recv(8)
            if Constant.SVR_RSP_USR_LOG_OUT_REP == rsp[0]:
                dialog = Dialog.Dialog(None, u"错误", u"该用户已经退出", 200, 150, 30, 60)
                dialog.Centre()
                dialog.Show()
        except Exception, e:
            dialog = Dialog.Dialog(None, u"错误", e.message, 200, 150, 30, 60)
            dialog.Centre()
            dialog.Show()
        finally:
            sock.close()
        os._exit(0)


# 点对点聊天的面板
class Panel_Single(wx.Panel):

    lc = None

    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        self.lc = wx.ListCtrl(self, size=(253, 410), style=wx.LC_REPORT)
        self.lc.InsertColumn(0, u"ID")
        self.lc.InsertColumn(1, u"用户名")
        self.lc.InsertColumn(2, u"状态")
        self.lc.SetColumnWidth(0, 50)
        self.lc.SetColumnWidth(1, 140)
        self.lc.SetColumnWidth(2, -2)
        for i in range(0, Login.FRIEND_NUM, 1):
            self.lc.InsertStringItem(i, u'%s' % unicode(Login.FRIEND_ID_ARRAY[i], 'utf-8'))
            self.lc.SetStringItem(i, 1, u'%s' % unicode(Login.FRIEND_NAME_ARRAY[i], 'utf-8'))
            if Constant.FRI_ONLINE == Login.FRIEND_STATUS_ARRAY[i]:
                self.lc.SetStringItem(i, 2, "ON")
            elif Constant.FRI_OFFLINE == Login.FRIEND_STATUS_ARRAY[i]:
                self.lc.SetStringItem(i, 2, "OFF")

        add_friend_button = wx.Button(self, label=u'添加好友', pos=(80, 420))

        self.Bind(wx.EVT_BUTTON, self.On_Add_Friend, add_friend_button)
        self.Bind(wx.EVT_LIST_ITEM_ACTIVATED, self.On_Chat_P2P)
        pass

    def On_Add_Friend(self, event):
        dialog = AddFriend.AddFriend(None)
        dialog.Centre()
        dialog.Show()

    def On_Chat_P2P(self, event):
        global P2P_ID
        global P2P_NAME

        index = event.GetIndex()
        item1 = self.lc.GetItem(index, 0)
        item2 = self.lc.GetItem(index, 1)
        P2P_ID = item1.GetText()
        P2P_NAME = item2.GetText()
        p2p_dialog = P2P_Group_Chat.P2P_Chat(None, P2P_NAME)
        p2p_dialog.Centre()
        p2p_dialog.Show()


# 群聊的面板
class Panel_Multi(wx.Panel):

    lc = None
    isSuccess = True

    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        self.lc = wx.ListCtrl(self, size=(253, 410), style=wx.LC_REPORT)
        self.lc.InsertColumn(0, u"群ID")
        self.lc.InsertColumn(1, u"群名称")
        self.lc.InsertColumn(2, u"成员数")
        self.lc.SetColumnWidth(0, 50)
        self.lc.SetColumnWidth(1, 140)
        self.lc.SetColumnWidth(2, -2)
        for i in range(0, Login.GROUP_NUM, 1):
            self.lc.InsertStringItem(i, u'%s' % unicode(Login.GROUP_ID_ARRAY[i], 'utf-8'))
            self.lc.SetStringItem(i, 1, u'%s' % unicode(Login.GROUP_NAME_ARRAY[i], 'utf-8'))
            self.lc.SetStringItem(i, 2, Login.GROUP_MEMBERNUM_ARRAY[i])

        add_group_button = wx.Button(self, label=u'创建群', pos=(80, 420))

        self.Bind(wx.EVT_BUTTON, self.On_Add_Group, add_group_button)
        self.Bind(wx.EVT_LIST_ITEM_ACTIVATED, self.On_Chat_Group)
        pass

    def On_Add_Group(self, event):
        dialog = AddGroup.AddGroup(None)
        dialog.Centre()
        dialog.Show()

    def On_Chat_Group(self, event):
        global GROUP_ID
        global GROUP_NAME
        global GROUP_MEM_NUM
        global GROUP_MEM_NAME_ARRAY

        index = event.GetIndex()
        item1 = self.lc.GetItem(index, 0)
        item2 = self.lc.GetItem(index, 1)
        GROUP_ID = item1.GetText()
        GROUP_NAME = item2.GetText()

        send_data = Constant.GRO_MEM_LST_REQ_SUC_RSP + \
            chr(len(str(Login.USERID))) + \
            chr(len(str(GROUP_ID))) + \
            str(Login.USERID) + \
            str(GROUP_ID)
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(10)
            sock.connect((Login.SERVER_IP, int(Login.SERVER_PORT)))
            sock.send(send_data)
            rsp = sock.recv(1024)
        except Exception, e:
            self.isSuccess = False
            dialog = Dialog.Dialog(None, u"错误", e.message, 200, 150, 30, 60)
            dialog.Centre()
            dialog.Show()
        finally:
            sock.close()

        if self.isSuccess:
            if Constant.GRO_MEM_LST_REQ_ERR_RSP == rsp[0]:
                dialog = Dialog.Dialog(None, u"错误", u"查询群组成员失败", 200, 150, 30, 60)
                dialog.Centre()
                dialog.Show()
            elif Constant.GRO_MEM_LST_REQ_SUC_RSP == rsp[0]:
                # 获取群组成员的数目
                index = 1
                length = ord(rsp[index])
                index += 1
                GROUP_MEM_NUM = int(rsp[index:(index + length)])
                index += length
                # 逐个解析出群组成员名字
                for i in range(0, GROUP_MEM_NUM, 1):
                    length = ord(rsp[index])
                    index += 1
                    GROUP_MEM_NAME_ARRAY[i] = rsp[index:(index + length)]
                    index += length
            else:
                dialog = Dialog.Dialog(None, u"错误", u"查询群组成员收到无效响应", 200, 150, 10, 60)
                dialog.Centre()
                dialog.Show()

        group_dialog = P2P_Group_Chat.Group_Chat(None, GROUP_NAME)
        group_dialog.Centre()
        group_dialog.Show()

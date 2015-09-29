#! /usr/bin/python
# encoding=utf-8

import wx
import os
import AddFriend
import Login
import Constant
import P2P_Chat

P2P_ID = ""
P2P_NAME = ""

class FriendList(wx.Frame):
    def __init__(self, parent):
        wx.Frame.__init__(self, parent,
                          title=u'局域网聊天',
                          size=(250, 500),
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
        os._exit(0)


class Panel_Single(wx.Panel):

    lc = None

    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        self.lc = wx.ListCtrl(self, size=(233, 410), style=wx.LC_REPORT)
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
        dialog = P2P_Chat.P2P_Chat(None)
        dialog.Centre()
        dialog.Show()


class Panel_Multi(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        pass

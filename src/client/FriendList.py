#! /usr/bin/python
# -*- coding: utf-8 -*-

import wx
import AddFriendDialog
import Login
import Constant

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


class Panel_Single(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        lc = wx.ListCtrl(self, size=(233, 410), style=wx.LC_REPORT)
        lc.InsertColumn(0, u"用户名")
        lc.InsertColumn(1, u"状态")
        lc.SetColumnWidth(0, 140)
        lc.SetColumnWidth(1, -2)
        for i in range(0, Login.FRIEND_NUM, 1):
            lc.InsertStringItem(i, u'%s' % unicode(Login.FRIEND_NAME_ARRAY[i], 'utf-8'))
            if Constant.FRI_ONLINE == Login.FRIEND_STATUS_ARRAY[i]:
                lc.SetStringItem(i, 1, "ON")
            elif Constant.FRI_OFFLINE == Login.FRIEND_STATUS_ARRAY[i]:
                lc.SetStringItem(i, 1, "OFF")

        add_friend_button = wx.Button(self, label=u'添加好友', pos=(80, 420))

        self.Bind(wx.EVT_BUTTON, self.On_Add_Friend, add_friend_button)
        self.Bind(wx.EVT_LIST_ITEM_RIGHT_CLICK, self.On_Chat_P2P)
        pass

    def On_Add_Friend(self, event):
        dialog = AddFriendDialog.AddFriendDialog(None)
        dialog.Centre()
        dialog.Show()
        pass

    def On_Chat_P2P(self, event):
        pass


class Panel_Multi(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        pass

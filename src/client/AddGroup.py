#! /usr/bin/python
# encoding=utf-8

import wx
import Login
import Dialog
import Constant


# 创建群对话框
class AddGroup(wx.Frame):

    groupName_edit = None
    check_list_box = None

    def __init__(self, parent):
        wx.Frame.__init__(self, parent,
                          title=u"创建群",
                          size=(200, 400),
                          style=wx.MINIMIZE_BOX
                                | wx.RESIZE_BORDER
                                | wx.SYSTEM_MENU
                                | wx.CAPTION
                                | wx.CLOSE_BOX
                                | wx.CLIP_CHILDREN)
        dPanel = wx.Panel(self)
        wx.StaticText(dPanel, label=u"群名：", pos=(25, 20))
        self.groupName_edit = wx.TextCtrl(dPanel, size=(100, 20), pos=(65, 15))
        self.groupName_edit.SetInsertionPoint(0)
        wx.StaticText(dPanel, label=u"请选择加入群的好友：", pos=(35, 50))
        choices_list = []
        for i in range(0, Login.FRIEND_NUM, 1):
            choices_list.append(Login.FRIEND_NAME_ARRAY[i])
        self.check_list_box = wx.CheckListBox(dPanel,
                                                choices=choices_list,
                                                pos=(25, 70),
                                                size=(140, 250))
        ok_button = wx.Button(dPanel, label=u'确定', pos=(55, 335))
        self.Bind(wx.EVT_BUTTON, self.On_Ok_Button, ok_button)

    def On_Ok_Button(self, event):

        sock = None
        rsp = None
        groupName = ""

        checklist = self.check_list_box.GetChecked()
        memNum = len(checklist)
        # 不允许建立空群组
        if 0 == memNum:
            dialog = Dialog.Dialog(None, u"错误", u"群内好友数不能为0", 200, 150, 60, 60)
            dialog.Centre()
            dialog.Show()

        groupName = self.groupName_edit.GetValue()
        memList = []
        memberString = ""
        for i in range(memNum):
            memList.append(Login.FRIEND_ID_ARRAY[i])
        for i in range(memNum):
            memberString += chr(len(memList[i]))
            memberString += memList[i]

        send_data = Constant.ADD_GRO_REQ_SUC_RSP + \
                chr(len(str(Login.USERID))) + \
                str(Login.USERID) + \
                chr(len(groupName.encode('utf-8'))) + \
                str(groupName.encode('utf-8')) + \
                chr(int(memNum)) + \
                str(int(memNum)) + \
                memberString

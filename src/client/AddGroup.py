# encoding=utf-8

import wx
import socket
import Login
import Dialog
import Constant


# 创建群对话框
class AddGroup(wx.Frame):

    groupName_edit = None
    check_list_box = None
    isSuccess = True

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
                chr(len(str(memNum))) + \
                str(int(memNum)) + \
                memberString

        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(10)
            sock.connect((Login.SERVER_IP, int(Login.SERVER_PORT)))
            sock.send(send_data)
            rsp = sock.recv(64)
        except Exception, e:
            dialog = Dialog.Dialog(None, u"错误", e.message, 200, 150, 30, 60)
            dialog.Centre()
            dialog.Show()
            self.isSuccess = False
        finally:
            sock.close()
        self.Close()

        if self.isSuccess:
            if Constant.SVR_RSP_REG_ERR_MAX_GRO == rsp[0]:
                dialog = Dialog.Dialog(None, u"错误", u"达到系统最大群数", 200, 150, 30, 60)
                dialog.Centre()
                dialog.Show()
            elif Constant.SVR_RSP_ADD_GRO_ERR_EXI == rsp[0]:
                dialog = Dialog.Dialog(None, u"错误", u"该群已经存在", 200, 150, 30, 60)
                dialog.Centre()
                dialog.Show()
            elif Constant.ADD_GRO_REQ_SUC_RSP == rsp[0]:
                length = ord(rsp[1])
                index = 2
                gid = int(rsp[index:(index + length)])
                dialog = Dialog.Dialog(None, u"信息", u"创建群成功，群ID=%d" % gid, 200, 150, 30, 60)
                dialog.Centre()
                dialog.Show()
            else:
                dialog = Dialog.Dialog(None, u"错误", u"创建群收到无效响应", 200, 150, 30, 60)
                dialog.Centre()
                dialog.Show()

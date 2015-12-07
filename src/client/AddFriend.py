# encoding=utf-8

import wx
import socket
import Dialog
import Tools
import Constant
import Login

# 添加好友对话框
class AddFriend(wx.Frame):

    friendID_edit = None
    isSuccess = True

    def __init__(self, parent):
        wx.Frame.__init__(self, parent,
                          title=u"添加好友",
                          size=(200, 150),
                          style=wx.MINIMIZE_BOX
                                | wx.RESIZE_BORDER
                                | wx.SYSTEM_MENU
                                | wx.CAPTION
                                | wx.CLOSE_BOX
                                | wx.CLIP_CHILDREN)
        dPanel = wx.Panel(self)
        wx.StaticText(dPanel, label=u"好友ID：", pos=(30, 40))
        self.friendID_edit = wx.TextCtrl(dPanel, size=(75, 20), pos=(85, 35))
        self.friendID_edit.SetInsertionPoint(0)
        ok_button = wx.Button(dPanel, label=u'确定', pos=(60, 80))

        self.Bind(wx.EVT_BUTTON, self.On_Ok_Button, ok_button)

    def On_Ok_Button(self, event):

        sock = None
        rsp = None
        friendID = self.friendID_edit.GetValue()

        # 对好友ID进行非空判断
        if "" == friendID:
            dialog = Dialog.Dialog(None, u"错误", u"好友ID不能为空", 200, 150, 60, 60)
            dialog.Centre()
            dialog.Show()
        # 校验好友ID是否为整数
        elif not Tools.idChecker(friendID):
            dialog = Dialog.Dialog(None, u"错误", u"好友ID不合法", 200, 150, 60, 60)
            dialog.Centre()
            dialog.Show()
        # 自己不能加自己为好友
        elif Login.USERID == friendID:
            dialog = Dialog.Dialog(None, u"错误", u"不允许添加自己为好友", 200, 150, 40, 60)
            dialog.Centre()
            dialog.Show()
        else:
            send_data = Constant.ADD_FRI_REQ_SUC_RSP + \
                chr(len(str(Login.USERID))) + \
                chr(len(str(friendID))) + \
                str(Login.USERID) + \
                str(friendID)
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
            self.Close()

            if self.isSuccess:
                if Constant.SVR_RSP_ADD_FRI_ERR_NOT_EXI == rsp[0]:
                    dialog = Dialog.Dialog(None, u"错误", u"该好友不存在", 200, 150, 40, 60)
                    dialog.Centre()
                    dialog.Show()
                elif Constant.SVR_RSP_ADD_FRI_ERR_AGN == rsp[0]:
                    dialog = Dialog.Dialog(None, u"错误", u"该好友已被添加", 200, 150, 40, 60)
                    dialog.Centre()
                    dialog.Show()
                elif Constant.ADD_FRI_REQ_SUC_RSP == rsp[0]:
                    dialog = Dialog.Dialog(None, u"信息", u"添加好友成功", 200, 150, 40, 60)
                    dialog.Centre()
                    dialog.Show()
                else:
                    dialog = Dialog.Dialog(None, u"错误", u"登录收到无效响应", 200, 150, 40, 60)
                    dialog.Centre()
                    dialog.Show()

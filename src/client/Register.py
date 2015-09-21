#! /usr/bin/python
# encoding=utf-8

import wx
import socket
import Tools
import Dialog
import Constant

# 注册对话框
class Register(wx.Frame):

    server_ip_edit = None
    server_port_edit = None
    username_edit = None
    password_edit = None
    isSuccess = True

    def __init__(self, parent):
        wx.Frame.__init__(self, parent,
                          title=u'用户注册',
                          size=(300, 250),
                          style=wx.MINIMIZE_BOX
                                | wx.RESIZE_BORDER
                                | wx.SYSTEM_MENU
                                | wx.CAPTION
                                | wx.CLOSE_BOX
                                | wx.CLIP_CHILDREN)
        register_panel = wx.Panel(self)

        # 服务器IP地址
        wx.StaticText(register_panel, label=u"服务器地址：", pos=(50, 25))
        self.server_ip_edit = wx.TextCtrl(register_panel, pos=(130, 20))
        self.server_ip_edit.SetInsertionPoint(0)

        # 服务器端口
        wx.StaticText(register_panel, label=u"服务器端口：", pos=(50, 65))
        self.server_port_edit = wx.TextCtrl(register_panel, pos=(130, 60))
        self.server_port_edit.SetInsertionPoint(0)

        # 用户姓名
        wx.StaticText(register_panel, label=u"真实姓名：", pos=(50, 105))
        self.username_edit = wx.TextCtrl(register_panel, pos=(130, 100))
        self.username_edit.SetInsertionPoint(0)

        # 用户密码
        wx.StaticText(register_panel, label=u"用户密码：", pos=(50, 145))
        self.password_edit = wx.TextCtrl(register_panel, pos=(130, 140), style=wx.TE_PASSWORD)
        self.password_edit.SetInsertionPoint(0)

        register_confirm_button = wx.Button(register_panel, label=u'确定', pos=(50, 180))
        register_cancel_button = wx.Button(register_panel, label=u'取消', pos=(165, 180))

        # 绑定按钮事件响应函数
        self.Bind(wx.EVT_BUTTON, self.On_Register_Confirm, register_confirm_button)
        self.Bind(wx.EVT_BUTTON, self.On_Register_Cancel, register_cancel_button)
        pass

    def On_Register_Confirm(self, event):
        global USERID
        server_ip = self.server_ip_edit.GetValue()
        server_port = self.server_port_edit.GetValue()
        username = self.username_edit.GetValue()
        password = self.password_edit.GetValue()
        sock = None
        rsp = None

        # 对登录界面产生的参数进行非空判断
        if "" == server_ip or "" == server_port or "" == username or "" == password:
            dialog = Dialog.Dialog(None, u"错误", u"参数不能为空", 200, 150, 60, 60)
            dialog.Centre()
            dialog.Show()
            self.isSuccess = False
        # 对输入的服务器IP地址进行合法性校验
        elif not Tools.ipChecker(server_ip):
            dialog = Dialog.Dialog(None, u"错误", u"IP地址不合法", 200, 150, 60, 60)
            dialog.Centre()
            dialog.Show()
            self.isSuccess = False
        # 校验输入的服务器端口是否为整数
        elif not Tools.portChecker(server_port):
            dialog = Dialog.Dialog(None, u"错误", u"端口不合法", 200, 150, 60, 60)
            dialog.Centre()
            dialog.Show()
            self.isSuccess = False
        else:
            send_data = Constant.REG_REQ_SUC_RSP + \
                chr(len(username.encode('utf-8'))) + \
                chr(len(password.encode('utf-8'))) + \
                username.encode('utf-8') + \
                password.encode('utf-8')
            try:
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                sock.connect((server_ip, int(server_port)))
                sock.send(send_data)
                rsp = sock.recv(8)
            except Exception, e:
                dialog = Dialog.Dialog(None, u"错误", e.message, 200, 150, 30, 60)
                dialog.Centre()
                dialog.Show()
                self.isSuccess = False
            finally:
                sock.close()
            self.Close()

            if self.isSuccess:
                if Constant.SVR_RSP_REG_ERR_MAX_USR == rsp[0]:
                    dialog = Dialog.Dialog(None, u"错误", u"注册的用户数已经满员", 200, 150, 30, 60)
                    dialog.Centre()
                    dialog.Show()
                elif Constant.SVR_RSP_REG_ERR_REP == rsp[0]:
                    dialog = Dialog.Dialog(None, u"错误", u"该用户已注册", 200, 150, 30, 60)
                    dialog.Centre()
                    dialog.Show()
                elif Constant.REG_REQ_SUC_RSP == rsp[0]:
                    USERID = int(rsp[1:])
                    dialog = Dialog.Dialog(None, u"通知", u"注册成功，ID=%d" % USERID, 200, 150, 30, 60)
                    dialog.Centre()
                    dialog.Show()
                else:
                    dialog = Dialog.Dialog(None, u"错误", u"注册收到无效响应", 200, 150, 30, 60)
                    dialog.Centre()
                    dialog.Show()

    def On_Register_Cancel(self, event):
        self.Close(True)

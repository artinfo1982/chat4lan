#! /usr/bin/python
# encoding=utf-8

import wx
import socket
import os
import Tools
import Dialog
import Constant
import Register
import FriendList
import P2P_Group_Chat

SERVER_IP = ""
SERVER_PORT = ""
LOCAL_PORT = ""
USERID = ""
USERNAME = ""

FRIEND_NUM = 0
FRIEND_ID_ARRAY = {}
FRIEND_NAME_ARRAY = {}
FRIEND_STATUS_ARRAY = {}

isParamOK = False

# 初始登录对话框
class Login(wx.Frame):

    server_ip_edit = None
    server_port_edit = None
    local_port_edit = None
    userid_edit = None
    password_edit = None
    isSuccess = True

    def __init__(self, parent):
        wx.Frame.__init__(self, parent,
                          title=u'局域网聊天',
                          size=(300, 250),
                          style=wx.MINIMIZE_BOX
                                | wx.RESIZE_BORDER
                                | wx.SYSTEM_MENU
                                | wx.CAPTION
                                | wx.CLOSE_BOX
                                | wx.CLIP_CHILDREN)
        login_panel = wx.Panel(self)

        # 服务器IP地址
        wx.StaticText(login_panel, label=u"服务器地址：", pos=(50, 25))
        self.server_ip_edit = wx.TextCtrl(login_panel, pos=(130, 20))
        self.server_ip_edit.SetInsertionPoint(0)

        # 服务器端口
        wx.StaticText(login_panel, label=u"服务器端口：", pos=(50, 55))
        self.server_port_edit = wx.TextCtrl(login_panel, pos=(130, 50))
        self.server_port_edit.SetInsertionPoint(0)

        # 本地监听端口
        wx.StaticText(login_panel, label=u"本地端口：", pos=(50, 85))
        self.local_port_edit = wx.TextCtrl(login_panel, pos=(130, 80))
        self.local_port_edit.SetInsertionPoint(0)

        # 用户ID
        wx.StaticText(login_panel, label=u"用户ID：", pos=(50, 115))
        self.userid_edit = wx.TextCtrl(login_panel, pos=(130, 110))
        self.userid_edit.SetInsertionPoint(0)

        # 用户密码
        wx.StaticText(login_panel, label=u"用户密码：", pos=(50, 145))
        self.password_edit = wx.TextCtrl(login_panel, pos=(130, 140), style=wx.TE_PASSWORD)
        self.password_edit.SetInsertionPoint(0)

        login_confirm_button = wx.Button(login_panel, label=u'登录', pos=(50, 180))
        login_register_button = wx.Button(login_panel, label=u'注册', pos=(165, 180))

        # 绑定按钮事件响应函数
        self.Bind(wx.EVT_BUTTON, self.OnLogin, login_confirm_button)
        self.Bind(wx.EVT_BUTTON, self.OnRegister, login_register_button)

        # 绑定关闭事件
        self.Bind(wx.EVT_CLOSE, self.On_Close)

    def OnLogin(self, event):
        # 将服务器信息和用户信息存入全局变量
        global SERVER_IP
        global SERVER_PORT
        global LOCAL_PORT
        global USERID
        global USERNAME
        global FRIEND_NUM
        global FRIEND_NAME_ARRAY
        global FRIEND_STATUS_ARRAY
        global isParamOK

        SERVER_IP = self.server_ip_edit.GetValue()
        SERVER_PORT = self.server_port_edit.GetValue()
        LOCAL_PORT = self.local_port_edit.GetValue()
        USERID = self.userid_edit.GetValue()
        password = self.password_edit.GetValue()
        sock = None
        rsp = None

        # 对登录界面产生的参数进行非空判断
        if "" == SERVER_IP or "" == SERVER_PORT or "" == USERID or "" == password:
            dialog = Dialog.Dialog(None, u"错误", u"参数不能为空", 200, 150, 60, 60)
            dialog.Centre()
            dialog.Show()
        # 对输入的服务器IP地址进行合法性校验
        elif not Tools.ipChecker(SERVER_IP):
            dialog = Dialog.Dialog(None, u"错误", u"IP地址不合法", 200, 150, 60, 60)
            dialog.Centre()
            dialog.Show()
        # 校验输入的服务器端口是否为整数
        elif not Tools.portChecker(SERVER_PORT):
            dialog = Dialog.Dialog(None, u"错误", u"端口不合法", 200, 150, 60, 60)
            dialog.Centre()
            dialog.Show()
        # 校验输入的用户ID是否为整数
        elif not Tools.idChecker(USERID):
            dialog = Dialog.Dialog(None, u"错误", u"用户ID不合法", 200, 150, 60, 60)
            dialog.Centre()
            dialog.Show()
        # 校验本地端口是否可用，若不可用，则更换一个
        elif not Tools.port_is_free(Tools.get_local_ip(), LOCAL_PORT):
            dialog = Dialog.Dialog(None, u"错误", u"本地端口被占用", 200, 150, 60, 60)
            dialog.Centre()
            dialog.Show()
        else:
            isParamOK = True
            send_data = Constant.LON_REQ_SUC_RSP + \
                chr(len(str(USERID))) + \
                chr(len(password.encode('utf-8'))) + \
                chr(len(P2P_Group_Chat.LOCAL_IP.encode('utf-8'))) + \
                chr(len(LOCAL_PORT.encode('utf-8'))) + \
                str(USERID) + \
                password.encode('utf-8') + \
                P2P_Group_Chat.LOCAL_IP.encode('utf-8') + \
                str(LOCAL_PORT)
            try:
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                sock.settimeout(10)
                sock.connect((SERVER_IP, int(SERVER_PORT)))
                sock.send(send_data)
                rsp = sock.recv(1024)
            except Exception, e:
                self.isSuccess = False
                dialog = Dialog.Dialog(None, u"错误", e.message, 200, 150, 30, 60)
                dialog.Centre()
                dialog.Show()
            finally:
                sock.close()
            self.Destroy()

            if self.isSuccess:
                if Constant.SVR_RSP_LON_ERR_REP == rsp[0]:
                    dialog = Dialog.Dialog(None, u"错误", u"鉴权失败", 200, 150, 30, 60)
                    dialog.Centre()
                    dialog.Show()
                elif Constant.SVR_RSP_LON_ERR_NOT_EXI == rsp[0]:
                    dialog = Dialog.Dialog(None, u"错误", u"用户不存在", 200, 150, 30, 60)
                    dialog.Centre()
                    dialog.Show()
                elif Constant.LON_REQ_SUC_RSP == rsp[0]:
                    # 获取自身的用户名
                    USERNAME = str(rsp[2:(2 + ord(rsp[1]))]).decode('utf-8')
                    index = ord(rsp[1]) + 2
                    # 获取好友总数
                    length = ord(rsp[index])
                    index += 1
                    FRIEND_NUM = int(rsp[index:(index + length)])
                    index += length
                    # 逐一解析并填充好友信息到字典数组备用
                    for i in range(0, FRIEND_NUM, 1):
                        length = ord(rsp[index])
                        index += 1
                        FRIEND_ID_ARRAY[i] = rsp[index:(index + length)]
                        index += length
                        length = ord(rsp[index])
                        index += 1
                        FRIEND_NAME_ARRAY[i] = rsp[index:(index + length)]
                        index += length
                        FRIEND_STATUS_ARRAY[i] = rsp[index]
                        index += 1
                    friend_list = FriendList.FriendList(None)
                    friend_list.Centre()
                    friend_list.Show()
                else:
                    dialog = Dialog.Dialog(None, u"错误", u"登录收到无效响应", 200, 150, 30, 60)
                    dialog.Centre()
                    dialog.Show()

    def OnRegister(self, event):
        register_frame = Register.Register(None)
        register_frame.Centre()
        register_frame.Show()

    def On_Close(self, event):
        os._exit(0)

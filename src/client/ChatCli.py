#! /usr/bin/python
# encoding=utf-8

import wx

# 全局变量定义区
SERVER_IP = None
SERVER_PORT = None
USERNAME = None
PASSWORD = None
FLAG_LOGIN = '\x01'
FLAG_REGISTER = '\x02'


# 校验IP格式的合法性
def ipChecker(ip_str):
    # 使用“.”分割输入的IP字符串
    address = ip_str.strip().split(".")
    # IP字符串使用“.”分割后只能有4个字段
    if len(address) != 4:
        return False
    else:
        for i in range(4):
            try:
                # 每一个分割后的字段都必须是整数
                address[i] = int(address[i])
            except Exception:
                return False
            # 每一个分割后的字段的数字都必须在0~255
            if address[i] < 0 or address[i] > 255:
                return False
            i += 1
        return True


# 校验端口的合法性
def portChecker():
    global SERVER_PORT
    try:
        # 判断端口是否为整数
        SERVER_PORT = int(SERVER_PORT)
    except Exception:
        return False
    # 判断端口范围是否0~65535
    if SERVER_PORT < 0 or SERVER_PORT > 65535:
        return False
    else:
        return True


# socket操作
def socketOperator(server_ip, server_port, flag, data):
    import socket
    import struct
    send_data = struct.pack("cs", flag, data)
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((server_ip, server_port))
        sock.send(send_data)
        return sock.recv(8192)
    except Exception, e:
        return e.message
    finally:
        sock.close()

# 对话框的通用类
class Dialog(wx.Frame):
    def __init__(self, parent, dTitle, dMessage, length, width, posX, posY):
        wx.Frame.__init__(self, parent,
                          title=dTitle,
                          size=(length, width),
                          style=wx.MINIMIZE_BOX
                                | wx.RESIZE_BORDER
                                | wx.SYSTEM_MENU
                                | wx.CAPTION
                                | wx.CLOSE_BOX
                                | wx.CLIP_CHILDREN)
        dPanel = wx.Panel(self)
        wx.StaticText(dPanel, label=dMessage, pos=(posX, posY))
        pass


# 初始登录对话框，暂不考虑密码鉴权
class Login_Frame(wx.Frame):

    server_ip_edit = None
    server_port_edit = None
    username_edit = None
    password_edit = None

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
        wx.StaticText(login_panel, label=u"服务器端口：", pos=(50, 65))
        self.server_port_edit = wx.TextCtrl(login_panel, pos=(130, 60))
        self.server_port_edit.SetInsertionPoint(0)

        # 用户姓名
        wx.StaticText(login_panel, label=u"用户姓名：", pos=(50, 105))
        self.username_edit = wx.TextCtrl(login_panel, pos=(130, 100))
        self.username_edit.SetInsertionPoint(0)

        # 用户密码
        wx.StaticText(login_panel, label=u"用户密码：", pos=(50, 145))
        self.password_edit = wx.TextCtrl(login_panel, pos=(130, 140), style=wx.TE_PASSWORD)
        self.password_edit.SetInsertionPoint(0)

        login_confirm_button = wx.Button(login_panel, label=u'登录', pos=(50, 180))
        login_register_button = wx.Button(login_panel, label=u'注册', pos=(165, 180))

        # 绑定按钮事件响应函数
        self.Bind(wx.EVT_BUTTON, self.OnLogin, login_confirm_button)
        self.Bind(wx.EVT_BUTTON, self.OnRegister, login_register_button)
        pass

    def OnLogin(self, event):
        # 将服务器信息和用户信息存入全局变量
        global SERVER_IP
        global SERVER_PORT
        global USERNAME
        global PASSWORD
        SERVER_IP = self.server_ip_edit.GetValue()
        SERVER_PORT = self.server_port_edit.GetValue()
        USERNAME = self.username_edit.GetValue()
        PASSWORD = self.password_edit.GetValue()

        # 对登录界面产生的参数进行非空判断
        if "" == SERVER_IP or "" == SERVER_PORT or "" == USERNAME:
            dialog = Dialog(None, u"错误", u"参数不能为空", 200, 150, 60, 60)
            dialog.Centre()
            dialog.Show()
        # 对输入的服务器IP地址进行合法性校验
        elif not ipChecker(SERVER_IP):
            dialog = Dialog(None, u"错误", u"IP地址不合法", 200, 150, 60, 60)
            dialog.Centre()
            dialog.Show()
        # 校验输入的服务器端口是否为整数
        elif not portChecker():
            dialog = Dialog(None, u"错误", u"端口不合法", 200, 150, 60, 60)
            dialog.Centre()
            dialog.Show()
        else:
            print socketOperator(SERVER_IP, SERVER_PORT, FLAG_LOGIN, "hello")
            self.Close()
            person_list_frame = Person_List_Frame(None)
            person_list_frame.Centre()
            person_list_frame.Show()

    def OnRegister(self, event):
        self.Close(True)


class Person_List_Frame(wx.Frame):
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
        notebook.AddPage(Panel_Single(notebook), u"单聊")
        notebook.AddPage(Panel_Multi(notebook), u"群聊")


class Panel_Single(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        wx.StaticText(self, label='AAA')
        pass


class Panel_Multi(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        wx.StaticText(self, label='BBB')
        pass


if __name__ == '__main__':
    app = wx.App()
    login_frame = Login_Frame(None)
    login_frame.Centre()
    login_frame.Show()
    app.MainLoop()

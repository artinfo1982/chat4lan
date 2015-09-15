#! /usr/bin/python
# encoding=utf-8

import wx

SERVER_IP = ""
SERVER_PORT = ""
USERNAME = ""

class Login_Frame(wx.Frame):

    server_ip_edit = None
    server_port_edit = None
    username_edit = None

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
        wx.StaticText(login_panel, label=u"服务器地址：", pos=(50, 35))
        self.server_ip_edit = wx.TextCtrl(login_panel, pos=(130, 30))
        self.server_ip_edit.SetInsertionPoint(0)

        # 服务器端口
        wx.StaticText(login_panel, label=u"服务器端口：", pos=(50, 75))
        self.server_port_edit = wx.TextCtrl(login_panel, pos=(130, 70))
        self.server_port_edit.SetInsertionPoint(0)

        # 用户姓名
        wx.StaticText(login_panel, label=u"用户名：", pos=(50, 115))
        self.username_edit = wx.TextCtrl(login_panel, pos=(130, 110))
        self.username_edit.SetInsertionPoint(0)

        login_confirm_button = wx.Button(login_panel,
                                         label=u'登录',
                                         pos=(50, 170))

        login_cancel_button = wx.Button(login_panel,
                                        label=u'取消',
                                        pos=(165, 170))

        # 绑定按钮事件响应函数
        self.Bind(wx.EVT_BUTTON, self.OnLogin, login_confirm_button)
        self.Bind(wx.EVT_BUTTON, self.OnCancel, login_cancel_button)

    def OnLogin(self, event):
        # 将服务器信息和用户信息存入全局变量
        global SERVER_IP
        global SERVER_PORT
        global USERNAME
        SERVER_IP = self.server_ip_edit.GetValue()
        SERVER_PORT = self.server_port_edit.GetValue()
        USERNAME = self.username_edit.GetValue()
        # 对登录界面产生的参数进行非空判断
        if "" != SERVER_IP and "" != SERVER_PORT and "" != USERNAME:
            self.Close(True)
            person_list_frame = Person_List_Frame(None)
            person_list_frame.Centre()
            person_list_frame.Show()

    def OnCancel(self, event):
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
    print SERVER_IP

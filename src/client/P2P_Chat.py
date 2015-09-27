#! /usr/bin/python
# encoding=utf-8

import wx
import socket
import FriendList
import Constant
import Login
import Dialog

class P2P_Chat(wx.Frame):

    msg_recv = None
    msg_send = None
    isSuccess = True

    def __init__(self, parent):
        wx.Frame.__init__(self, parent,
                          title=u'%s' % FriendList.P2P_NAME,
                          size=(500, 500),
                          style=wx.MINIMIZE_BOX
                                | wx.RESIZE_BORDER
                                | wx.SYSTEM_MENU
                                | wx.CAPTION
                                | wx.CLOSE_BOX
                                | wx.CLIP_CHILDREN)
        p2p_panel = wx.Panel(self)
        self.msg_recv = wx.TextCtrl(p2p_panel, size=(450, 300), style=wx.TE_READONLY)
        self.msg_recv.SetBackgroundColour(wx.Colour(192, 220, 192))
        self.msg_send = wx.TextCtrl(p2p_panel, size=(450, 100), style=wx.TE_PROCESS_ENTER)
        self.msg_send.SetBackgroundColour(wx.Colour(192, 220, 192))
        vbox = wx.BoxSizer(wx.VERTICAL)
        vbox.Add(self.msg_recv,
                 proportion=0,
                 flag=wx.UP | wx.LEFT | wx.RIGHT | wx.EXPAND,
                 border=5)
        vbox.Add((-1, 20))
        vbox.Add(self.msg_send,
                 proportion=1,
                 flag=wx.Bottom | wx.LEFT | wx.RIGHT | wx.EXPAND,
                 border=5)
        p2p_panel.SetSizer(vbox)

        self.msg_send.Bind(wx.EVT_TEXT_ENTER, self.On_Send_Clear, self.msg_send)
        pass

    def On_Send_Clear(self, event):
        rsp = None
        msg = self.msg_send.GetValue().encode('utf-8')
        send_data = Constant.SEND_RECV_REQ_SUC_RSP + \
            chr(len(str(FriendList.P2P_ID))) + \
            chr(len(msg)) + \
            str(FriendList.P2P_ID) + \
            msg
        self.msg_send.Clear()
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect((Login.SERVER_IP, int(Login.SERVER_PORT)))
            sock.send(send_data)
            rsp = sock.recv(8192)
        except Exception, e:
            self.isSuccess = False
            dialog = Dialog.Dialog(None, u"错误", e.message, 200, 150, 30, 60)
            dialog.Centre()
            dialog.Show()
        finally:
            sock.close()

        if self.isSuccess:
            if Constant.SEND_RECV_REQ_SUC_RSP == rsp[0]:
                print "OK"


class Panel_Single(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        pass

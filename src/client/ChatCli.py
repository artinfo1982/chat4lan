#! /usr/bin/python
# encoding=utf-8

import wx

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

app = wx.App()

frame = wx.Frame(None,
                 title=u"局域网聊天",
                 size=(250, 500),
                 # 禁用最大化
                 style=wx.MINIMIZE_BOX
                       | wx.RESIZE_BORDER
                       | wx.SYSTEM_MENU
                       | wx.CAPTION
                       | wx.CLOSE_BOX
                       | wx.CLIP_CHILDREN)
frame.Centre()

notebook = wx.Notebook(frame)
notebook.AddPage(Panel_Single(notebook), u"单聊")
notebook.AddPage(Panel_Multi(notebook), u"群聊")
frame.Show()

app.MainLoop()

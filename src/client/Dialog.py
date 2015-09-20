#! /usr/bin/python
# encoding=utf-8

import wx

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

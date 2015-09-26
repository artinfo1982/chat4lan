#! /usr/bin/python
# encoding=utf-8

import wx
import Login

if __name__ == '__main__':
    app = wx.App()
    login = Login.Login(None)
    login.Centre()
    login.Show()
    app.MainLoop()

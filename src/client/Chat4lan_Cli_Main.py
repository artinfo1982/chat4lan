#! /usr/bin/python
# encoding=utf-8

"""
register:
client --> (flag[1], username_len[1], password_len[1], username[?], password[?])
server --> (flag[1], userid[1] or space[1])

login:
client --> (flag[1], password_len[1], userid[1], password[?])
server --> (flag[1], list_size[1], list[?])
"""

import wx
import Login

if __name__ == '__main__':
    app = wx.App()
    login = Login.Login(None)
    login.Centre()
    login.Show()
    app.MainLoop()

#! /usr/bin/python
# encoding=utf-8

import wx
import threading
import Login
import MsgRecver

class GUI(threading.Thread):
    def run(self):
        app = wx.App()
        login = Login.Login(None)
        login.Centre()
        login.Show()
        app.MainLoop()

def main():
    t_gui = GUI()
    t_gui.start()
    t_msgRecver = MsgRecver.MsgRecv()
    t_msgRecver.start()

if __name__ == '__main__':
    main()

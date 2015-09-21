# chat4lan
Chat4lan 是一款局域网内的聊天工具，有两种模式的版本。一种是没有图形界面的(src/chat_noGUI.c)，一种是带GUI ，采用C/S模式，客户端使用Python编写，服务端采用C语言编写(epoll + deamon)。

客户端文件列表说明：＜/br＞
Chat4lan_Cli_Main.py  ---   客户端主程序main函数入口＜/br＞
Constant.py           ---   常量定义文件(包括自定义协议列表)＜/br＞
Dialog.py             ---   通用对话框基类＜/br＞
FriendList.py         ---   好友列表模块＜/br＞
Login.py              ---   登录模块＜/br＞
Register.py           ---   注册模块＜/br＞
Tools.py              ---   工程使用到的一些工具，例如IP合法性校验、端口合法性校验等＜/br＞

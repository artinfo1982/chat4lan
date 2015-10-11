# chat4lan
Chat4lan 是一款局域网内的聊天工具，有两种模式的版本。一种是没有图形界面的(src/chat_noGUI.c)，一种是带GUI ，采用C/S模式，客户端使用Python + wxPython编写(包含图形界面)，服务端采用C语言编写(epoll + deamon)。   


实现的功能有：   
注册   
登录   
添加好友   
好友列表展示   
单聊(点对点聊天)   
创建群组   
群组内添加成员   
群聊   
心跳   
用户是否在线的状态刷新   
   
   
客户端和服务器端的通信接口详见doc目录中的接口说明。   
   
   
本程序遵循GPL规则发行。

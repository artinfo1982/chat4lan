# chat4lan
一、简介   
Chat4lan 是一款局域网内的聊天工具。采用C/S+DB模式设计，客户端使用Python语言编写(图形界面GUI使用wxpython库)，服务端完全采用C语言编写。用户核心数据存储在mysql / mariadb数据库中。   


二、实现的功能      
注册   
登录   
添加好友   
好友列表展示   
单聊(点对点聊天)   
创建群组   
群组内添加成员   
群聊   
退出   
心跳   
   
   
三、相关技术   
1.epoll   
服务端采用epoll，监听网络事件，为后续解决单机大数量级长连接(100万+)铺平道路。   
2.多进程   
服务端采用多进程架构，进程间相互独立，即使某个进程异常退出，对其他进程也不会产生影响。   
3.daemon   
服务端daemon化运行，前台终端零I/O，所有行为都记录在后台日志文件中。   
4.watchdog   
服务端daemon进程为其fork出来的各个业务子进程提供watchdog能力，如果子进程异常退出，daemon进程会自动将其拉起。   
5.mysql / mariadb存储用户数据   
用户的核心数据存储在mysql / mariadb中。   


四、安装及使用   
1.在linux上安装mysql / mariadb。   
mysql的下载、安装及使用方法请访问其官方网站http://www.mysql.com。   
mariadb的下载、安装及使用方法请访问其官方网站http://www.mariadb.org。   
2.登录mysql / mariadb，创建一个名为chat的用户，用户密码为chat，创建一个名为chatDB的表空间，载入src/sql/createTables.sql，为mysql / mariadb创建表。   
3.将src/server下的所有程序上传到linux的任意路径下，运行make，出现chat4lan的可执行文件后，运行./chat4lan ip port。   
4.在windows客户机上安装python 2.7，配置环境变量。Python的下载、安装及配置请访问其官方网站https://www.python.org。   
5.在windows客户机上安装py2exe，目的是将python脚本编译为windows环境下的可执行程序以及动态库（\*.exe、\*.dll），此后可以将这些可执行程序以及动态库打包至任意一台没有python的windows机器上运行。Py2exe的下载、安装请访问其官方网站http://www.py2exe.org。   
6.将src/client下的所有程序上传到windows客户机的任意路径下，在dos窗口进入源代码路径，运行python setup.py p2exe，在源代码路径下会出现一个名为dist的文件夹，其中的Main.exe即为最终的客户端可执行程序，双击即可运行。   


五、注意事项   
1.为了确保mysql / mariadb在使用的过程中不出现中文乱码，需要将字符集统一设置为utf8。   
在my.cnf中添加以下字段：   
[mysqld]   
character-set-server=utf8   
default-character-set=utf8   
collation-server=utf8_unicode_ci   
[mysql]   
default-character-set=utf8   
[client]   
default-character-set=utf8   
重启mysql / mariadb，以mysql为例：/etc/init.d/mysqld restart 或者 service mysqld restart。   
2.如果安装mysql，需要安装mysql-server、mysql-client、mysql-devel。   


六、Changelog   
2015-12-13， v0.1， 主体功能开发、端到端自测试完毕，交付项目组内部公测。v0.1版本包含注册、登录、添加好友、创建群组、点对点聊天、群聊、退出、心跳的功能。   


七、联系方式   
在使用本软件系统时遇到的各种问题，可以将问题和建议发送至 artinfo1982@126.com，看到后会及时答复。   


八、其他说明   
1.本程序遵循GPL规则发行，严禁将本代码用于任何商业用途。   
2.客户端和服务器端的通信接口详见doc目录中的接口说明。   

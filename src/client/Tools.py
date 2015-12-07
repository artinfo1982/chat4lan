# encoding=utf-8

import socket

# 校验IP格式的合法性
def ipChecker(ip_str):
    # 使用“.”分割输入的IP字符串
    address = ip_str.strip().split(".")
    # IP字符串使用“.”分割后只能有4个字段
    if len(address) != 4:
        return False
    else:
        for i in range(4):
            try:
                # 每一个分割后的字段都必须是整数
                address[i] = int(address[i])
            except Exception:
                return False
            # 每一个分割后的字段的数字都必须在0~255
            if address[i] < 0 or address[i] > 255:
                return False
            i += 1
        return True


# 校验端口的合法性
def portChecker(port):
    try:
        # 判断端口是否为整数
        port = int(port)
    except Exception:
        return False
    # 判断端口范围是否0~65535
    if port < 0 or port > 65535:
        return False
    else:
        return True


# 校验用户ID的合法性
def idChecker(user_id):
    try:
        # 判断端口是否为整数
        user_id = int(user_id)
    except Exception:
        return False
    # 判断ID范围是否>0
    if user_id < 0:
        return False
    else:
        return True

# 校验本地端口是否可用
def port_is_free(ip, port):
    port = int(port)
    s = socket.socket()
    s.settimeout(0.5)
    try:
        return s.connect_ex((ip, port)) != 0
    finally:
        s.close()

# 获取本机IP
def get_local_ip():
    return socket.gethostbyname(socket.gethostname())

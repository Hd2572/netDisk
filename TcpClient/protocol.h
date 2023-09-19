#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef unsigned int uint;
/*
    通讯协议
*/

#define REGIST_OK "regist ok"                        //注册成功
#define REGIST_FAILED "regist failed: name existed"  //注册失败，用户名重复

#define LOGIN_OK "login ok"                                              //登录成功
#define LOGIN_FAILED "login failed: name error or pwd error or relogin"  //登录失败，用户名密码错误或已在线

enum ENUM_MSG_TYPE  //消息类型
{
    ENUM_MSG_TYPE_MIN = 0,
    ENUM_MSG_TYPE_REGIST_REQUEST,  //注册请求1
    ENUM_MSG_TYPE_REGIST_RESPOND,  //注册回复2

    ENUM_MSG_TYPE_LOGIN_REQUEST,  //登录请求3
    ENUM_MSG_TYPE_LOGIN_RESPOND,  //登录回复4

    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST,  //在线用户查询5
    ENUM_MSG_TYPE_ALL_ONLINE_RESPOND,  //在线用户回复6

    ENUM_MSG_TYPE_SEARCH_USR_REQUEST,  //搜索用户请求7
    ENUM_MSG_TYPE_SEARCH_USR_RESPOND,  //搜索用户请求8
    //    ENUM_MSG_TYPE_REQUEST = 0,
    //    ENUM_MSG_TYPE_RESPOND = 0,
    ENUM_MSG_TYPE_MAX = 0x00ffffff,

};

struct PDU
{
    uint uiPDULen;    //总协议单元大小
    uint uiMsgType;   //消息类型
    char caData[64];  //文件名
    uint uiMsgLen;    //实际消息长度
    int caMsg[];      //实际消息
};

PDU* mkPDU(uint uiMsgLen);  //创建空间

#endif  // PROTOCOL_H

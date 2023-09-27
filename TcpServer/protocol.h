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

#define SEARCH_USR_NO "no such people"  //用户不存在
#define SEARCH_USR_ONLINE "online"      //在线
#define SEARCH_USR_OFFLINE "offline"    //不在线

#define UNKNOW_ERROR "unknow error"          //未知错误
#define EXISTED_FRIEND "friend exist"        //好友已存在
#define ADD_FRIEND_OFFLINE "usr offline"     //用户不在线
#define ADD_FRIEND_NO_EXIST "usr not exist"  //用户不存在

#define DEL_FRIEND_OK "delete friend ok"  //成功删除好友

#define DIR_NO_EXIST "cur dir not exist"   //文件夹不存在
#define FILE_NAME_EXIST "file name exist"  //文件夹重名
#define CREAT_DIR_OK "create dir ok"       //创建文件夹成功

#define DEL_DIR_OK "delete dir ok"                               //删除文件夹成功
#define DEL_DIR_FAILURED "delete dir failured: is reguler file"  //删除失败是常规文件

#define RENAME_FILE_OK "rename file ok"              //重命名成功
#define RENAME_FILE_FAILURED "rename file failured"  //重命名失败

#define ENTER_DIR_FAILURED "enter dir failured: is reguler file"  //进入文件夹失败，是常规文件

#define DEL_FILE_OK "delete file ok"                           //删除常规文件成功
#define DEL_FILE_FAILURED "delete file failured: is diretory"  //删除常规文件失败，是文件夹

#define UPLOAD_FILE_OK "upload file ok"              //上传文件成功
#define UPLOAD_FILE_FAILURED "upload file failured"  //上传文件失败

#define MOVE_FILE_OK "move file ok"
#define MOVE_FILE_FAILURED "move file failured:is reguler file"

#define COMMON_ERR "operate failed: system is busy"  //系统忙

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

    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,  //添加好友请求9
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,  //添加好友回复10

    ENUM_MSG_TYPE_ADD_FRIEND_AGGREE,  //同意添加好友11
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE,  //拒绝添加好友12

    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST,  //刷新好友请求13
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND,  //刷新好友回复14

    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST,  //删除好友请求15
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND,  //删除好友回复16

    ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST,  //私聊请求17
    ENUM_MSG_TYPE_PRIVATE_CHAT_RESPOND,  //私聊回复18

    ENUM_MSG_TYPE_GROUP_CHAT_REQUEST,  //群聊请求19
    ENUM_MSG_TYPE_GROUP_CHAT_RESPOND,  //群聊回复20

    ENUM_MSG_TYPE_CREATE_DIR_REQUEST,  //创建文件夹请求21
    ENUM_MSG_TYPE_CREATE_DIR_RESPOND,  //创建文件夹回复22

    ENUM_MSG_TYPE_FLUSH_FILE_REQUEST,  //刷新文件请求23
    ENUM_MSG_TYPE_FLUSH_FILE_RESPOND,  //刷新文件回复24

    ENUM_MSG_TYPE_DEL_DIR_REQUEST,  //删除目录请求25
    ENUM_MSG_TYPE_DEL_DIR_RESPOND,  //删除目录回复26

    ENUM_MSG_TYPE_RENAME_FILE_REQUEST,  //重命名文件请求27
    ENUM_MSG_TYPE_RENAME_FILE_RESPOND,  //重命名文件回复28

    ENUM_MSG_TYPE_ENTER_DIR_REQUEST,  //进入文件夹请求29
    ENUM_MSG_TYPE_ENTER_DIR_RESPOND,  //进入文件夹回复30

    ENUM_MSG_TYPE_DEL_FILE_REQUEST,  //删除常规文件请求31
    ENUM_MSG_TYPE_DEL_FILE_RESPOND,  //删除常规文件回复32

    ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST,  //上传文件请求33
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND,  //上传文件回复34

    ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST,  //下载文件请求35
    ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND,  //下载文件回复36

    ENUM_MSG_TYPE_SHARE_FILE_REQUEST,  //共享文件请求37
    ENUM_MSG_TYPE_SHARE_FILE_RESPOND,  //共享文件回复38

    ENUM_MSG_TYPE_SHARE_FILE_NOTE,          // 39
    ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND,  // 40

    ENUM_MSG_TYPE_MOVE_FILE_REQUEST,  //移动文件请求41
    ENUM_MSG_TYPE_MOVE_FILE_RESPOND,  //移动文件回复42

    ENUM_MSG_TYPE_MAX = 0x00ffffff,

};

struct FileInfo
{
    char caFileName[32];  //文件名字
    int iFileType;        //文件类型
};

struct PDU
{
    uint uiPDULen;    //总协议单元大小
    uint uiMsgType;   //消息类型
    char caData[64];  //文件名
    uint uiMsgLen;    //实际消息长度
    int caMsg[];      //实际消息
};

PDU* mkPDU(uint uiMsgLen);  //创建一个pdu

#endif  // PROTOCOL_H

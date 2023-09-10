#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef unsigned int uint;

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

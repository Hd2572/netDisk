#include "mytcpsocket.h"

#include "mytcpserver.h"

MyTcpSocket::MyTcpSocket()
{
    connect(this, SIGNAL(readyRead()), this, SLOT(recvMsg()));           //本身的readyread，本身的recvMsg处理
    connect(this, SIGNAL(disconnected()), this, SLOT(clientOffline()));  //关联客户端下线槽
}

QString MyTcpSocket::getName()  //获取用户名
{
    return m_strName;
}

void MyTcpSocket::recvMsg()  //接收readyread
{
    // qDebug() << this->bytesAvailable();  //打印获得的数据大小

    uint uiPDULen = 0;
    this->read((char*)&uiPDULen, sizeof(uint));                      //全部大小
    uint uiMsgLen = uiPDULen - sizeof(PDU);                          //数据大小
    PDU* pdu = mkPDU(uiMsgLen);                                      //创建新pdu
    this->read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));  //读数据

    switch (pdu->uiMsgType)  //判断消息类型
    {
        case ENUM_MSG_TYPE_REGIST_REQUEST:  //注册请求
        {
            char caName[32] = {'\0'};
            char caPwd[32] = {'\0'};
            strncpy(caName, pdu->caData, 32);
            strncpy(caPwd, pdu->caData + 32, 32);  //获取用户名密码

            // qDebug() << caName << caPwd << pdu->uiMsgType;

            bool ret = OpeDB::getInstance().handleRegist(caName, caPwd);  //数据库处理

            PDU* respdu = mkPDU(0);                            //回复pdu
            respdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;  //注册回复

            if (ret)  //成功
            {
                strcpy(respdu->caData, REGIST_OK);
            }
            else  //失败
            {
                strcpy(respdu->caData, REGIST_FAILED);
            }

            write((char*)respdu, respdu->uiPDULen);  // socket发送
            free(respdu);                            //释放空间
            respdu = NULL;

            break;
        }
        case ENUM_MSG_TYPE_LOGIN_REQUEST:  //登录请求
        {
            char caName[32] = {'\0'};
            char caPwd[32] = {'\0'};
            strncpy(caName, pdu->caData, 32);
            strncpy(caPwd, pdu->caData + 32, 32);  //获取用户名密码

            bool ret = OpeDB::getInstance().handleLogin(caName, caPwd);  //登录

            PDU* respdu = mkPDU(0);                           //回复pdu
            respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;  //登录回复

            if (ret)  //成功
            {
                strcpy(respdu->caData, LOGIN_OK);
                m_strName = caName;  //保存用户名
            }
            else  //失败
            {
                strcpy(respdu->caData, LOGIN_FAILED);
            }

            write((char*)respdu, respdu->uiPDULen);  // socket发送
            free(respdu);                            //释放空间
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:  //查在线用户请求
        {
            QStringList ret = OpeDB::getInstance().handleAllOnline();  //查询
            uint uiMsgLen = ret.size() * 32;

            // qDebug() << "请求：" << pdu->uiMsgType;

            PDU* respdu = mkPDU(uiMsgLen);  //回复pdu
            respdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
            for (int i = 0; i < ret.size(); i++)  //拷贝
            {
                memcpy((char*)respdu->caMsg + i * 32, ret.at(i).toStdString().c_str(), ret.at(i).size());
            }
            write((char*)respdu, respdu->uiPDULen);  // socket发送

            // qDebug() << "回复：" << respdu->uiMsgType;

            free(respdu);  //释放空间
            respdu = NULL;

            break;
        }
        case ENUM_MSG_TYPE_SEARCH_USR_REQUEST:  //查找用户请求
        {
            int ret = OpeDB::getInstance().handleSearchUsr(pdu->caData);  //查找
            PDU* respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_RESPOND;
            if (-1 == ret)  //没找到
            {
                strcpy(respdu->caData, SEARCH_USR_NO);
            }
            else if (1 == ret)  //在线
            {
                strcpy(respdu->caData, SEARCH_USR_ONLINE);
            }
            else if (0 == ret)  //离线
            {
                strcpy(respdu->caData, SEARCH_USR_OFFLINE);
            }
            write((char*)respdu, respdu->uiPDULen);  //发送
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:  //添加好友请求
        {
            char caPerName[32] = {'\0'};  //目标用户名
            char caName[32] = {'\0'};     //申请方用户名
            strncpy(caPerName, pdu->caData, 32);
            strncpy(caName, pdu->caData + 32, 32);

            int ret = OpeDB::getInstance().handleAddFriend(caPerName, caName);  //数据库处理

            PDU* respdu = NULL;  //回复pdu
            if (-1 == ret)       //未知错误
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData, UNKNOW_ERROR);
                write((char*)respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }
            else if (0 == ret)  //已经是好友
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData, EXISTED_FRIEND);
                write((char*)respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }
            else if (1 == ret)  //对方在线
            {
                // qDebug() << "申请方：" << caName << "对方" << caPerName << endl;
                MyTcpServer::getInstance().resend(caPerName, pdu);  //好友申请转发给对方
            }
            else if (2 == ret)  //对方不在线
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData, ADD_FRIEND_OFFLINE);
                write((char*)respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }
            else if (3 == ret)  //对方用户名不存在
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData, ADD_FRIEND_NO_EXIST);
                write((char*)respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE:  //同意加好友
        {
            char caPerName[32] = {'\0'};
            char caName[32] = {'\0'};
            strncpy(caPerName, pdu->caData, 32);                           //对方名
            strncpy(caName, pdu->caData + 32, 32);                         //申请人名
            OpeDB::getInstance().handleAgreeAddFriend(caPerName, caName);  //数据库操作

            MyTcpServer::getInstance().resend(caName, pdu);  //转发给申请人
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:  //拒绝加好友
        {
            char caName[32] = {'\0'};
            strncpy(caName, pdu->caData + 32, 32);           //申请人名
            MyTcpServer::getInstance().resend(caName, pdu);  //转发给申请人
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:  //刷新好友列表
        {
            char caName[32] = {'\0'};
            strncpy(caName, pdu->caData, 32);  //请求方用户名

            QStringList ret = OpeDB::getInstance().handleFlushFriend(caName);  //数据库查询，返回好友列表

            uint uiMsgLen = ret.size() * 32;  //回复pdu大小
            PDU* respdu = mkPDU(uiMsgLen);

            respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;

            for (int i = 0; i < ret.size(); i++)  //复制好友用户名
            {
                memcpy((char*)(respdu->caMsg) + i * 32, ret.at(i).toStdString().c_str(), ret.at(i).size());
            }

            write((char*)respdu, respdu->uiPDULen);  //发给客户端
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:  //删除好友请求
        {
            char caSelfName[32] = {'\0'};
            char caFriendName[32] = {'\0'};

            strncpy(caSelfName, pdu->caData, 32);                            //申请人用户名
            strncpy(caFriendName, pdu->caData + 32, 32);                     //要删除的好友
            OpeDB::getInstance().handleDelFriend(caSelfName, caFriendName);  //数据库处理

            PDU* respdu = mkPDU(0);  // pdu
            respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
            strcpy(respdu->caData, DEL_FRIEND_OK);

            write((char*)respdu, respdu->uiPDULen);  //发送
            free(respdu);
            respdu = NULL;

            MyTcpServer::getInstance().resend(caFriendName, pdu);  //转发给被删除方，没在线则找不到不转发

            break;
        }
        default: break;
    }

    free(pdu);
    pdu = NULL;

    // qDebug() << caName << caPwd << pdu->uiMsgType;
}

void MyTcpSocket::clientOffline()  //客户端下线
{
    OpeDB::getInstance().handleOffline(m_strName.toStdString().c_str());  //更改在线状态
    emit offline(this);                                                   //发送下线信号
}

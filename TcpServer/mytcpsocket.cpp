#include "mytcpsocket.h"

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

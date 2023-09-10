#include "mytcpserver.h"

MyTcpServer::MyTcpServer() {}

MyTcpServer& MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)  //有连接自动调用
{
    qDebug() << "new client connect";
    MyTcpSocket* pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(socketDescriptor);  //创建socket
    m_tcpSockeList.append(pTcpSocket);                  //存入列表
}

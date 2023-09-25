#include "mytcpserver.h"

MyTcpServer::MyTcpServer() {}

MyTcpServer& MyTcpServer::getInstance()  //返回同一个静态的对象
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)  //有连接自动调用
{
    qDebug() << "new client connect";
    MyTcpSocket* pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(socketDescriptor);  //创建socket

    m_tcpSockeList.append(pTcpSocket);  //存入列表

    //    QThread* thread = new QThread();                                    // 1
    //    connect(pTcpSocket, SIGNAL(disconnected()), thread, SLOT(quit()));  // 2

    connect(pTcpSocket, SIGNAL(offline(MyTcpSocket*)), this, SLOT(deleteSocket(MyTcpSocket*)));  //关联下线

    //    pTcpSocket->moveToThread(thread);  // 3

    //    thread->start();  // 4

    //    emit newConnection();  // 5
}

void MyTcpServer::resend(const char* pername, PDU* pdu)  //重新发送
{
    if (NULL == pername || NULL == pdu)  //判空
    {
        return;
    }

    QString strName = pername;
    for (int i = 0; i < m_tcpSockeList.size(); i++)  //遍历已有socket找到目标
    {
        if (strName == m_tcpSockeList.at(i)->getName())  //找到目标
        {
            m_tcpSockeList.at(i)->write((char*)pdu, pdu->uiPDULen);  //发送
            break;
        }
    }
    //没找到则退出，不转发
}

void MyTcpServer::deleteSocket(MyTcpSocket* mysocket)  //删除已有socket
{
    QList<MyTcpSocket*>::iterator iter = m_tcpSockeList.begin();  //找到目标socket删除
    for (; iter != m_tcpSockeList.end(); iter++)
    {
        if (mysocket == *iter)
        {
            (*iter)->deleteLater();  //延迟释放
            *iter = NULL;
            m_tcpSockeList.erase(iter);
            break;
        }
    }
    // for (int i = 0; i < m_tcpSockeList.size(); i++) { qDebug() << m_tcpSockeList.at(i)->getName(); } //打印剩余socket
}

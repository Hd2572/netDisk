#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QDebug>
#include <QList>
#include <QTcpServer>
#include <QThread>

#include "mytcpsocket.h"
/*
    自定义的tcpserver类
*/

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
    //引入moc，可以使用qt机制

  public:
    MyTcpServer();
    static MyTcpServer& getInstance();                  //一个静态的mytcpserver对象
    void incomingConnection(qintptr socketDescriptor);  //重载父类虚函数,检测到connect自动调用

  public slots:
    void deleteSocket(MyTcpSocket* mysocket);  //删除已有socket

  private:
    QList<MyTcpSocket*> m_tcpSockeList;  //已有socket指针列表
};

#endif  // MYTCPSERVER_H

#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H
#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QTcpSocket>

#include "opedb.h"
#include "protocol.h"
class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
  public:
    MyTcpSocket();
    QString getName();  //获取名字

  signals:
    void offline(MyTcpSocket* mysocket);  //下线信号

  public slots:
    void recvMsg();        //接收消息
    void clientOffline();  //客户端下线

  private:
    QString m_strName;  //该socket对应的客户端用户名
};

#endif  // MYTCPSOCKET_H

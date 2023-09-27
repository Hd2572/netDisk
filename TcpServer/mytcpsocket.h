#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfoList>
#include <QTcpSocket>
#include <QTimer>

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
    QFile m_file;       //文件
    qint64 m_iTotal;    //文件大小
    qint64 m_iRecved;   //已经接收多少
    bool m_bUpload;     //是否正在上传文件

    QTimer* m_pTimer;
};

#endif  // MYTCPSOCKET_H

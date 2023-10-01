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
    QString getName();                                    //获取名字
    void copyDir(QString strSrcDir, QString strDestDir);  //拷贝文件夹

  signals:
    void offline(MyTcpSocket* mysocket);  //下线信号

  public slots:
    void recvMsg();           //接收消息
    void clientOffline();     //客户端下线
    void sendFileToClient();  //发送文件给客户端

  private:
    QString m_strName;  //该socket对应的客户端用户名
    QFile m_file;       //文件操作对象
    qint64 m_iTotal;    //文件大小
    qint64 m_iRecved;   //已经接收多少
    bool m_bUpload;     //是否正在上传文件

    QTimer* m_pTimer;  //计时器
};

#endif  // MYTCPSOCKET_H

#ifndef TCPCLIENT_H
#define TCPCLIENT_H

/*
    客户端登录窗口
*/

#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QHostAddress>
#include <QMessageBox>
#include <QTcpSocket>
#include <QWidget>

#include "opewidget.h"
#include "protocol.h"

namespace Ui
{
    class TcpClient;
}

class TcpClient : public QWidget
{
    Q_OBJECT

  public:
    explicit TcpClient(QWidget* parent = nullptr);
    ~TcpClient();

    void loadConfig();                //加载配置文件
    static TcpClient& getInstance();  //单例
    QTcpSocket& getTcpSocket();       //返回socket

    QString loginName();                  //返回当前登录用户名
    QString curPath();                    //返回当前路径
    void setCurPath(QString strCurPath);  //设置当前路径

  public slots:
    void showConnect();  //连接成功弹窗
    void recvMsg();      //

  private slots:
    // void on_send_pd_clicked();测试

    void on_login_pb_clicked();  //登录按钮

    void on_regist_pb_clicked();  //注册按钮

    void on_cancel_pb_clicked();  //注销按钮

  private:
    Ui::TcpClient* ui;
    QString m_strIP;   // ip
    quint16 m_usport;  //端口

    QTcpSocket m_tcpSocket;  //套接字对象，连接服务器、与服务器交互

    QString m_strLoginName;  //我的用户名

    QString m_strCurPath;  //当前路径
    QFile m_file;          //操作的文件对象
};

#endif  // TCPCLIENT_H

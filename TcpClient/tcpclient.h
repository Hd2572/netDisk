#ifndef TCPCLIENT_H
#define TCPCLIENT_H

/*
    客户端
*/

#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QHostAddress>
#include <QMessageBox>
#include <QTcpSocket>
#include <QWidget>

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

    void loadConfig();  //加载配置文件
  public slots:
    void showConnect();  //连接成功弹窗

  private slots:
    void on_send_pd_clicked();

  private:
    Ui::TcpClient* ui;
    QString m_strIP;
    quint16 m_usport;

    QTcpSocket m_tcpSocket;  //套接字对象，连接服务器、与服务器交互
};

#endif  // TCPCLIENT_H

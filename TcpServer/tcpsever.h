#ifndef TCPSEVER_H
#define TCPSEVER_H

/*
       服务器端

*/

#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QHostAddress>
#include <QMessageBox>
#include <QWidget>

#include "mytcpserver.h"

namespace Ui
{
    class TcpSever;
}

class TcpSever : public QWidget
{
    Q_OBJECT

  public:
    explicit TcpSever(QWidget* parent = nullptr);
    ~TcpSever();
    void loadConfig();  //加载配置文件

  private:
    Ui::TcpSever* ui;
    QString m_strIP;
    quint16 m_usport;
};

#endif  // TCPSEVER_H

#include "tcpsever.h"

#include "ui_tcpsever.h"

TcpSever::TcpSever(QWidget* parent) : QWidget(parent), ui(new Ui::TcpSever)
{
    ui->setupUi(this);
    loadConfig();                                                        //加载配置文件
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP), m_usport);  //创建一个自定义的tcpserver对象，并监听端口
}

TcpSever::~TcpSever() { delete ui; }

void TcpSever::loadConfig()
{
    QFile file(":/server.config");       //打开配置文件
    if (file.open(QIODevice::ReadOnly))  //只读
    {
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();  //转为string保存

        file.close();

        strData.replace("\r\n", " ");
        QStringList strList = strData.split(" ");  //空格分割赋值给ip和端口

        m_strIP = strList.at(0);
        m_usport = strList.at(1).toUShort();
        // qDebug() << "ip:" << m_strIP << "port:" << m_usport;
    }
    else
    {
        QMessageBox::critical(this, "open config", "open config failed");
    }
}

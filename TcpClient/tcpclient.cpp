#include "tcpclient.h"

#include "ui_tcpclient.h"

TcpClient::TcpClient(QWidget* parent) : QWidget(parent), ui(new Ui::TcpClient)  //构造
{
    ui->setupUi(this);
    loadConfig();  //加载配置文件

    connect(&m_tcpSocket, SIGNAL(connected()), this, SLOT(showConnect()));  //连接连接成功信号与弹窗槽

    m_tcpSocket.connectToHost(QHostAddress(m_strIP), m_usport);  //连接服务器，成功则发送connect信号
}

TcpClient::~TcpClient() { delete ui; }

void TcpClient::loadConfig()  //加载配置文件
{
    QFile file(":/client.config");       //打开配置文件
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

void TcpClient::showConnect()  //连接成功弹窗
{
    QMessageBox::information(this, "连接服务器", "连接服务器成功");
}

void TcpClient::on_send_pd_clicked()  //发送按钮槽
{
    QString strMsg = ui->lineEdit->text();  //获取输入框数据
    if (!strMsg.isEmpty())
    {
        PDU* pdu = mkPDU(strMsg.toUtf8().size() + 1);  //申请地址
        pdu->uiMsgType = 8888;
        memcpy(pdu->caMsg, strMsg.toStdString().c_str(), strMsg.toUtf8().size());  //拷贝
        // qDebug() << pdu->uiPDULen << (char*)(pdu->caMsg);
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
        free(pdu);  //释放空间
        pdu = NULL;
    }
    else
    {
        QMessageBox::warning(this, "信息发生", "发送的消息不能为空");
    }
}

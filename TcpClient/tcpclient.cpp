#include "tcpclient.h"

#include "ui_tcpclient.h"

TcpClient::TcpClient(QWidget* parent) : QWidget(parent), ui(new Ui::TcpClient)  //构造
{
    ui->setupUi(this);

    resize(500, 250);  //重设窗口大小
    loadConfig();      //加载配置文件

    connect(&m_tcpSocket, SIGNAL(connected()), this, SLOT(showConnect()));  //连接连接成功信号与弹窗槽
    connect(&m_tcpSocket, SIGNAL(readyRead()), this, SLOT(recvMsg()));      //连接接收服务器信息槽

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

void TcpClient::recvMsg()  //接收数据
{
    uint uiPDULen = 0;
    m_tcpSocket.read((char*)&uiPDULen, sizeof(uint));                      //全部大小
    uint uiMsgLen = uiPDULen - sizeof(PDU);                                //数据大小
    PDU* pdu = mkPDU(uiMsgLen);                                            //创建新pdu
    m_tcpSocket.read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));  //读数据

    switch (pdu->uiMsgType)  //判断消息类型
    {
        case ENUM_MSG_TYPE_REGIST_RESPOND:  //注册回复
        {
            if (strcmp(pdu->caData, REGIST_OK) == 0)  //弹窗
            {
                QMessageBox::information(this, "注册", REGIST_OK);
            }
            else if (strcmp(pdu->caData, REGIST_FAILED) == 0)
            {
                QMessageBox::warning(this, "注册", REGIST_FAILED);
            }

            break;
        }
        case ENUM_MSG_TYPE_LOGIN_RESPOND:  //登录回复
        {
            if (strcmp(pdu->caData, LOGIN_OK) == 0)  //弹窗
            {
                QMessageBox::information(this, "登录", LOGIN_OK);
            }
            else if (strcmp(pdu->caData, LOGIN_FAILED) == 0)
            {
                QMessageBox::warning(this, "登录", LOGIN_FAILED);
            }

            break;
        }
        default: break;
    }

    free(pdu);
    pdu = NULL;
}

#if 0  //测试函数注释掉
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
#endif
void TcpClient::on_login_pb_clicked()  //登录按钮
{
    QString strName = ui->name_le->text();  //获取用户名密码
    QString strPwd = ui->pwd_le->text();
    if (!strName.isEmpty() && !strPwd.isEmpty())
    {
        PDU* pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;             //消息类型：登录请求
        strncpy(pdu->caData, strName.toStdString().c_str(), 32);  //将用户名密码存到caData
        strncpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);

        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);  // socket发送
        free(pdu);                                     //释放空间
        pdu = NULL;
    }
    else
    {
        QMessageBox::critical(this, "登录", "登录失败:用户名或密码为空");
    }
}

void TcpClient::on_regist_pb_clicked()  //注册按钮
{
    QString strName = ui->name_le->text();  //获取用户名密码
    QString strPwd = ui->pwd_le->text();
    if (!strName.isEmpty() && !strPwd.isEmpty())
    {
        PDU* pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST;            //消息类型：注册请求
        strncpy(pdu->caData, strName.toStdString().c_str(), 32);  //将用户名密码存到caData
        strncpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);

        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);  // socket发送
        free(pdu);                                     //释放空间
        pdu = NULL;
    }
    else
    {
        QMessageBox::critical(this, "注册", "注册失败:用户名或密码为空");
    }
}

void TcpClient::on_cancel_pb_clicked()  //注销按钮
{

}

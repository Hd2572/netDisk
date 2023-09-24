#include "privatechat.h"

#include <QMessageBox>

#include "protocol.h"
#include "tcpclient.h"
#include "ui_privatechat.h"

PrivateChat::PrivateChat(QWidget* parent) : QWidget(parent), ui(new Ui::PrivateChat) { ui->setupUi(this); }

PrivateChat::~PrivateChat() { delete ui; }

PrivateChat& PrivateChat::getInstance()  //获取单例
{
    static PrivateChat instance;
    return instance;
}

void PrivateChat::setChatName(QString strName)  //设置私聊双方用户名
{
    m_strChatName = strName;
    m_strLoginName = TcpClient::getInstance().loginName();
}

void PrivateChat::updateMsg(const PDU* pdu)  //更新消息窗口
{
    if (NULL == pdu)  //判空
    {
        return;
    }
    char caSendName[32] = {'\0'};  //发送方名字
    memcpy(caSendName, pdu->caData, 32);
    QString strMsg = QString("%1 says: %2").arg(caSendName).arg((char*)(pdu->caMsg));
    ui->showMsg_te->append(strMsg);  //添加消息
}

void PrivateChat::on_sendMsg_pb_clicked()  //发送信息按钮
{
    QString strMsg = ui->inputMsg_le->text();  //获取信息
    ui->inputMsg_le->clear();                  //清空输入框

    if (!strMsg.isEmpty())  //信息非空
    {
        PDU* pdu = mkPDU(strMsg.toUtf8().size() + 1);  // pdu
        pdu->uiMsgType = ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST;

        memcpy(pdu->caData, m_strLoginName.toStdString().c_str(), m_strLoginName.size());  //拷贝交流双方与信息
        memcpy(pdu->caData + 32, m_strChatName.toStdString().c_str(), m_strChatName.size());

        strcpy((char*)(pdu->caMsg), strMsg.toStdString().c_str());

        // memcpy(pdu->caMsg, strMsg.toStdString().c_str(), strMsg.toUtf8().size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);  //发送
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::warning(this, "私聊", "发送的聊天信息不能为空");
    }
}

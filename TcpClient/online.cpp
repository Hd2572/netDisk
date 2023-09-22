#include "online.h"

#include "tcpclient.h"
#include "ui_online.h"

Online::Online(QWidget* parent) : QWidget(parent), ui(new Ui::Online) { ui->setupUi(this); }

Online::~Online() { delete ui; }

void Online::showUsr(PDU* pdu)  //显示在线
{
    ui->online_lw->clear();  //清空已有列表
    if (NULL == pdu)
    {
        return;
    }
    uint uiSize = pdu->uiMsgLen / 32;  //拷贝次数
    char caTmp[32];
    for (uint i = 0; i < uiSize; i++)
    {
        memcpy(caTmp, (char*)pdu->caMsg + i * 32, 32);
        ui->online_lw->addItem(caTmp);
    }
}

void Online::on_addFriend_pb_clicked()  //添加好友
{
    QListWidgetItem* pItem = ui->online_lw->currentItem();  //当前选中用户

    if (pItem == nullptr)  //判空
        return;

    QString strPerUsrName = pItem->text();                        //用户名
    QString strLoginName = TcpClient::getInstance().loginName();  //自己的用户名

    if (strPerUsrName == strLoginName)
    {
        QMessageBox::information(this, "添加好友", "不能添加自己为好友");
        return;  //不能添加自己
    }

    // qDebug() << "申请方：" << strLoginName << "对方" << strPerUsrName << endl;

    PDU* pdu = mkPDU(0);  //封装pdu
    pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;

    memcpy(pdu->caData, strPerUsrName.toStdString().c_str(), strPerUsrName.size());
    memcpy(pdu->caData + 32, strLoginName.toStdString().c_str(), strLoginName.size());

    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);  //发送pdu

    free(pdu);
    pdu = NULL;
}

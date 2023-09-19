#include "online.h"

#include "ui_online.h"

Online::Online(QWidget* parent) : QWidget(parent), ui(new Ui::Online) { ui->setupUi(this); }

Online::~Online() { delete ui; }

void Online::showUsr(PDU* pdu)  //显示在线
{
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

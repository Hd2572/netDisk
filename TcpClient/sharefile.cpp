#include "sharefile.h"

#include <QDebug>

#include "opewidget.h"
#include "tcpclient.h"

ShareFile::ShareFile(QWidget* parent) : QWidget(parent)
{
    m_pSelectAllPB = new QPushButton("全选");         //全选按钮
    m_pCancelSelectPB = new QPushButton("取消选择");  //取消选择按钮

    m_pOKPB = new QPushButton("确定");      //确定按钮
    m_pCancelPB = new QPushButton("取消");  //取消按钮

    m_pSA = new QScrollArea;                        //好友展示区域
    m_pFriendW = new QWidget;                       //好友窗口
    m_pFriendWVBL = new QVBoxLayout(m_pFriendW);    //好友窗口垂直布局
    m_pButtonGroup = new QButtonGroup(m_pFriendW);  //管理好友窗口名字按钮
    m_pButtonGroup->setExclusive(false);            //选择不互斥

    QCheckBox* pCB = new QCheckBox("text");  //好友名
    // qDebug() << "friend:" << pCB->text() << endl;
    m_pFriendWVBL->addWidget(pCB);   //加入好友
    m_pButtonGroup->addButton(pCB);  //加入框选
    m_pSA->setWidget(m_pFriendW);    //设置好友窗口

    QHBoxLayout* pTopHBL = new QHBoxLayout;  //水平布局
    pTopHBL->addWidget(m_pSelectAllPB);
    pTopHBL->addWidget(m_pCancelSelectPB);
    pTopHBL->addStretch();  //置于最左

    QHBoxLayout* pDownHBL = new QHBoxLayout;  //水平布局
    pDownHBL->addWidget(m_pOKPB);
    pDownHBL->addWidget(m_pCancelPB);

    QVBoxLayout* pMainVBL = new QVBoxLayout;  //两个水平布局与好友区域垂直布局
    pMainVBL->addLayout(pTopHBL);
    pMainVBL->addWidget(m_pSA);
    pMainVBL->addLayout(pDownHBL);
    setLayout(pMainVBL);  //设置总布局

    connect(m_pCancelSelectPB, SIGNAL(clicked(bool)), this, SLOT(cancelSelect()));  //取消选

    connect(m_pSelectAllPB, SIGNAL(clicked(bool)), this, SLOT(selectAll()));  //全选

    connect(m_pOKPB, SIGNAL(clicked(bool)), this, SLOT(okShare()));  //确定分享

    connect(m_pCancelPB, SIGNAL(clicked(bool)), this, SLOT(cancelShare()));  //取消分享
}

ShareFile& ShareFile::getInstance()  //获取单例
{
    static ShareFile instance;
    return instance;
}

void ShareFile::updateFriend(QListWidget* pFriendList)  //更新好友
{
    if (NULL == pFriendList)  //指针为空
    {
        return;
    }
    QAbstractButton* tmp = NULL;
    QList<QAbstractButton*> preFriendList = m_pButtonGroup->buttons();  //获取之前好友

    for (int i = 0; i < preFriendList.size(); i++)
    {
        tmp = preFriendList[i];
        m_pFriendWVBL->removeWidget(tmp);   //垂直布局移除原本好友
        m_pButtonGroup->removeButton(tmp);  //框选组移除原本好友
        // preFriendList.removeOne(tmp);       //列表中移除
        delete tmp;  //释放原本按钮
        tmp = NULL;
    }
    QCheckBox* pCB = NULL;

    // qDebug() << "shareupdate" << pFriendList->count() << endl;
    for (int i = 0; i < pFriendList->count(); i++)  //新获得的列表项加入
    {
        pCB = new QCheckBox(pFriendList->item(i)->text());  //好友名
        // qDebug() << "friend:" << pCB->text() << endl;
        m_pFriendWVBL->addWidget(pCB);   //加入好友
        m_pButtonGroup->addButton(pCB);  //加入框选
    }
    m_pSA->setWidget(m_pFriendW);  //设置好友窗口
}

void ShareFile::cancelSelect()  //取消选择
{
    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();  //获取列表
    for (int i = 0; i < cbList.size(); i++)
    {
        if (cbList[i]->isChecked())  //设置未选中
        {
            cbList[i]->setChecked(false);
        }
    }
}

void ShareFile::selectAll()  //全选
{
    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
    for (int i = 0; i < cbList.size(); i++)
    {
        if (!cbList[i]->isChecked())  //设置选中
        {
            cbList[i]->setChecked(true);
        }
    }
}

void ShareFile::okShare()  //确定分享
{
    QString strName = TcpClient::getInstance().loginName();                             //登录名
    QString strCurPath = TcpClient::getInstance().curPath();                            //当前路径
    QString strShareFileName = OpeWidget::getInstance().getBook()->getShareFileName();  //分享文件名

    QString strPath = strCurPath + "/" + strShareFileName;  //拼接路径

    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
    int num = 0;
    for (int i = 0; i < cbList.size(); i++)  //选中了多少个
    {
        if (cbList[i]->isChecked())
        {
            num++;
        }
    }

    PDU* pdu = mkPDU(32 * num + strPath.toUtf8().size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    sprintf(pdu->caData, "%s %d", strName.toStdString().c_str(), num);  //分享文件名

    int j = 0;
    for (int i = 0; i < cbList.size(); i++)  //分享的好友名
    {
        if (cbList[i]->isChecked())
        {
            memcpy((char*)(pdu->caMsg) + j * 32, cbList[i]->text().toStdString().c_str(),
                   cbList[i]->text().toUtf8().size());
            j++;
        }
    }

    memcpy((char*)(pdu->caMsg) + num * 32, strPath.toStdString().c_str(), strPath.toUtf8().size());  //文件路径

    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);  //发送

    free(pdu);
    pdu = NULL;
}

void ShareFile::cancelShare()  //取消分享
{
    hide();  //隐藏窗口
}

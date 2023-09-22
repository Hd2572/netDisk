#include "friend.h"

#include <QInputDialog>

#include "protocol.h"
#include "tcpclient.h"

Friend::Friend(QWidget* parent) : QWidget(parent)
{
    m_pShowMsgTE = new QTextEdit;           //显示信息
    m_pFriendListWidget = new QListWidget;  //好友列表
    m_pInputMsgLE = new QLineEdit;          //信息输入框

    m_pDelFriendPB = new QPushButton("删除好友");          //删除好友按钮
    m_pFlushFriendPB = new QPushButton("刷新好友");        //刷新在线好友按钮
    m_pShowOnlineUsrPB = new QPushButton("显示在线用户");  //显示在线用户按钮
    m_pSearchUsrPB = new QPushButton("查找用户");          //搜索用户按钮
    m_pMsgSendPB = new QPushButton("信息发送");            //群聊发信息按钮
    m_pPrivateChatPB = new QPushButton("私聊");            //私聊发信息按钮

    QVBoxLayout* pRighttPBVBL = new QVBoxLayout;  //最右侧按钮垂直布局
    pRighttPBVBL->addWidget(m_pDelFriendPB);
    pRighttPBVBL->addWidget(m_pFlushFriendPB);  //五个按钮加入垂直布局
    pRighttPBVBL->addWidget(m_pShowOnlineUsrPB);
    pRighttPBVBL->addWidget(m_pSearchUsrPB);
    pRighttPBVBL->addWidget(m_pPrivateChatPB);

    QHBoxLayout* pTopHBL = new QHBoxLayout;  //信息框与好友列表按钮水平布局
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pRighttPBVBL);

    QHBoxLayout* pMsgHBL = new QHBoxLayout;  //信息输入框与发送按钮水平布局
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);

    m_pOnline = new Online;  // online窗口

    QVBoxLayout* pMain = new QVBoxLayout;  //总的垂直布局
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();  //在线用户一开始隐藏

    setLayout(pMain);  //设置总的布局

    connect(m_pShowOnlineUsrPB, SIGNAL(clicked(bool)), this, SLOT(showOnline()));  //显示在线用户槽
    connect(m_pSearchUsrPB, SIGNAL(clicked(bool)), this, SLOT(searchUsr()));       //搜索用户
    connect(m_pFlushFriendPB, SIGNAL(clicked(bool)), this, SLOT(flushFriend()));   //刷新好友列表
    connect(m_pDelFriendPB, SIGNAL(clicked(bool)), this, SLOT(delFriend()));       //删除好友
}

void Friend::showAllOnlineUsr(PDU* pdu)  //展示在线
{
    if (NULL == pdu)
    {
        return;
    }
    m_pOnline->showUsr(pdu);
}

QListWidget* Friend::getFriendList()  //获取好友列表
{
    return m_pFriendListWidget;
}

void Friend::updateFriendList(PDU* pdu)  //刷新在线好友列表
{
    if (NULL == pdu)  //判空
    {
        return;
    }

    uint uiSize = pdu->uiMsgLen / 32;  //在线人数
    char caName[32] = {'\0'};
    for (uint i = 0; i < uiSize; i++)  //接收数据
    {
        memcpy(caName, (char*)(pdu->caMsg) + i * 32, 32);
        m_pFriendListWidget->addItem(caName);
    }
}

void Friend::showOnline()  //显示/隐藏在线用户按钮
{
    if (m_pOnline->isHidden())
    {
        m_pOnline->show();

        PDU* pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);  //发送查询请求
        // qDebug() << "friend 请求" << pdu->uiMsgType;
        free(pdu);
    }
    else
    {
        m_pOnline->hide();
    }
}

void Friend::searchUsr()  //搜索用户
{
    m_strSearchName = QInputDialog::getText(this, "搜索", "用户名:");  //用户名输入弹窗
    if (!m_strSearchName.isEmpty())
    {
        // qDebug() << m_strSearchName;
        PDU* pdu = mkPDU(0);
        memcpy(pdu->caData, m_strSearchName.toStdString().c_str(), m_strSearchName.size());  //名字存入pdu
        pdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_REQUEST;

        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);  //发送pdu
        free(pdu);
        pdu = NULL;
    }
}

void Friend::flushFriend()  //刷新好友列表按钮
{
    m_pFriendListWidget->clear();  //清空当前

    QString strName = TcpClient::getInstance().loginName();  //获取用户名

    PDU* pdu = mkPDU(0);  // pdu
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    memcpy(pdu->caData, strName.toStdString().c_str(), strName.size());  //写入用户名

    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);  //发送
    free(pdu);
    pdu = NULL;
}

void Friend::delFriend()  //删除好友按钮
{
    if (NULL != m_pFriendListWidget->currentItem())  //当前选中好友
    {
        QString strFriendName = m_pFriendListWidget->currentItem()->text();  //好友名字

        PDU* pdu = mkPDU(0);  // pdu
        pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
        QString strSelfName = TcpClient::getInstance().loginName();  //我的名字

        memcpy(pdu->caData, strSelfName.toStdString().c_str(), strSelfName.size());
        memcpy(pdu->caData + 32, strFriendName.toStdString().c_str(), strFriendName.size());

        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);  //发送
        free(pdu);
        pdu = NULL;
    }
}

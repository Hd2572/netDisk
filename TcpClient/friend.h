#ifndef FRIEND_H
#define FRIEND_H

#include <QHBoxLayout>  //水平布局
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>  //垂直布局
#include <QWidget>

#include "online.h"

/*
        好友窗口

*/

class Friend : public QWidget
{
    Q_OBJECT
  public:
    explicit Friend(QWidget* parent = nullptr);
    void showAllOnlineUsr(PDU* pdu);  //展示在线
    QListWidget* getFriendList();     //获取好友列表
    void updateFriendList(PDU* pdu);  //刷新在线好友列表
    void updateGroupMsg(PDU* pdu);    //更新群消息

    QString m_strSearchName;  //要查找的查找用户名

  signals:

  public slots:
    void showOnline();   //显示/隐藏在线用户按钮
    void searchUsr();    //查找用户按钮
    void flushFriend();  //刷新好友按钮
    void delFriend();    //删除好友按钮
    void privateChat();  //好友私聊按钮
    void groupChat();    //群聊按钮

  private:
    QTextEdit* m_pShowMsgTE;           //显示信息
    QListWidget* m_pFriendListWidget;  //好友列表
    QLineEdit* m_pInputMsgLE;          //信息输入框

    QPushButton* m_pDelFriendPB;      //删除好友按钮
    QPushButton* m_pFlushFriendPB;    //刷新在线好友按钮
    QPushButton* m_pShowOnlineUsrPB;  //显示在线用户按钮
    QPushButton* m_pSearchUsrPB;      //搜索用户按钮

    QPushButton* m_pMsgSendPB;      //群聊按钮
    QPushButton* m_pPrivateChatPB;  //私聊按钮

    Online* m_pOnline;  // online列表
};

#endif  // FRIEND_H

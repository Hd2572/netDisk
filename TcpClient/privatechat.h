#ifndef PRIVATECHAT_H
#define PRIVATECHAT_H

#include <QWidget>

#include "protocol.h"
/*、
私聊窗口

*/

namespace Ui
{
    class PrivateChat;
}

class PrivateChat : public QWidget
{
    Q_OBJECT

  public:
    explicit PrivateChat(QWidget* parent = nullptr);
    ~PrivateChat();

    static PrivateChat& getInstance();  //单例

    void setChatName(QString strName);  //聊天对象用户名
    void updateMsg(const PDU* pdu);

  private slots:
    void on_sendMsg_pb_clicked();  //发送信息按钮

  private:
    Ui::PrivateChat* ui;
    QString m_strChatName;   //聊天对象用户名
    QString m_strLoginName;  //我的用户名
};

#endif  // PRIVATECHAT_H

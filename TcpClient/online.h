#ifndef ONLINE_H
#define ONLINE_H

#include <QWidget>

#include "protocol.h"

/*
    用户在线窗口
*/
namespace Ui
{
    class Online;
}

class Online : public QWidget
{
    Q_OBJECT

  public:
    explicit Online(QWidget* parent = nullptr);
    ~Online();

    void showUsr(PDU* pdu);  //显示在线

  private slots:
    void on_addFriend_pb_clicked();  //添加好友

  private:
    Ui::Online* ui;
};

#endif  // ONLINE_H

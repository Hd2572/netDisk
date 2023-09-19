#ifndef OPEWIDGET_H
#define OPEWIDGET_H

#include <QListWidget>
#include <QStackedWidget>  //堆栈窗口
#include <QWidget>

#include "book.h"
#include "friend.h"

/*
    窗口操作
*/

class OpeWidget : public QWidget
{
    Q_OBJECT
  public:
    explicit OpeWidget(QWidget* parent = nullptr);
    static OpeWidget& getInstance();  //返回单例
    Friend* getFriend();              //获得好友界面

  signals:

  public slots:

  private:
    QListWidget* m_pListW;  //窗口列表
    Friend* m_pFriend;      //好友窗口
    Book* m_pBook;          //图书窗口

    QStackedWidget* m_pSW;  //窗口每次只显示一个
};

#endif  // OPEWIGET_H

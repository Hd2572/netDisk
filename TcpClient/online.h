#ifndef ONLINE_H
#define ONLINE_H

#include <QWidget>
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

  private:
    Ui::Online* ui;
};

#endif  // ONLINE_H

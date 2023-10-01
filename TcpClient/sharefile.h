#ifndef SHAREFILE_H
#define SHAREFILE_H

#include <QButtonGroup>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>  //按钮组
#include <QScrollArea>  //区域
#include <QVBoxLayout>
#include <QWidget>
/*
    文件分享窗口
*/

class ShareFile : public QWidget
{
    Q_OBJECT
  public:
    explicit ShareFile(QWidget* parent = nullptr);
    static ShareFile& getInstance();              //获取单例
    void updateFriend(QListWidget* pFriendList);  //更新好友

  signals:

  public slots:
    void cancelSelect();  //取消选择
    void selectAll();     //全选

    void okShare();      //确定分享
    void cancelShare();  //取消分享

  private:
    QPushButton* m_pSelectAllPB;     //全选按钮
    QPushButton* m_pCancelSelectPB;  //取消选择按钮

    QPushButton* m_pOKPB;      //确定按钮
    QPushButton* m_pCancelPB;  //取消按钮

    QScrollArea* m_pSA;            //好友展示区域
    QWidget* m_pFriendW;           //好友窗口
    QVBoxLayout* m_pFriendWVBL;    //好友窗口垂直布局
    QButtonGroup* m_pButtonGroup;  //好友名字框选组
};

#endif  // SHAREFILE_H

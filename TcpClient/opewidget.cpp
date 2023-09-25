#include "opewidget.h"

OpeWidget::OpeWidget(QWidget* parent) : QWidget(parent)
{
    m_pListW = new QListWidget(this);  //窗口列表
    m_pListW->addItem("好友");
    m_pListW->addItem("图书");

    m_pFriend = new Friend;  //创建好友、图书窗口
    m_pBook = new Book;

    m_pSW = new QStackedWidget;  //堆栈窗口
    m_pSW->addWidget(m_pFriend);
    m_pSW->addWidget(m_pBook);

    QHBoxLayout* pMain = new QHBoxLayout;  //水平布局
    pMain->addWidget(m_pListW);
    pMain->addWidget(m_pSW);

    setLayout(pMain);  //设置布局

    connect(m_pListW, SIGNAL(currentRowChanged(int)), m_pSW, SLOT(setCurrentIndex(int)));  //根据左侧列表更换显示窗口
}

OpeWidget& OpeWidget::getInstance()  //返回单例
{
    static OpeWidget instance;
    return instance;
}

Friend* OpeWidget::getFriend()  //获得好友页面
{
    return m_pFriend;
}

Book* OpeWidget::getBook()  //获得图书界面
{
    return m_pBook;
}

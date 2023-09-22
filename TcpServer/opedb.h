#ifndef OPEDB_H
#define OPEDB_H

#include <QDebug>
#include <QMessageBox>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>
/*
数据库操作
*/

class OpeDB : public QObject
{
    Q_OBJECT
  public:
    explicit OpeDB(QObject* parent = nullptr);
    static OpeDB& getInstance();  //生成一个静态的数据库单例
    void init();                  //初始化函数
    ~OpeDB();

    bool handleRegist(const char* name, const char* pwd);  //处理注册请求
    bool handleLogin(const char* name, const char* pwd);   //处理注册请求
    void handleOffline(const char* name);                  //处理用户下线
    QStringList handleAllOnline();                         //查找所有在线用户
    int handleSearchUsr(const char* name);                 //查找用户

    int handleAddFriend(const char* pername, const char* name);        //处理添加好友
    void handleAgreeAddFriend(const char* pername, const char* name);  //同意加好友
    QStringList handleFlushFriend(const char* name);                   //处理刷新好友列表
    bool handleDelFriend(const char* name, const char* friendName);    //处理删除好友

  signals:

  public slots:

  private:
    QSqlDatabase m_db;  //连接数据库
};

#endif  // OPEDB_H

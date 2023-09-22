#include "opedb.h"

OpeDB::OpeDB(QObject* parent) : QObject(parent) { m_db = QSqlDatabase::addDatabase("QSQLITE"); }

OpeDB& OpeDB::getInstance()  //返回同一个静态对象的引用
{
    static OpeDB instance;
    return instance;
}

void OpeDB::init()
{
    m_db.setHostName("localhost");                                                     //数据库地址
    m_db.setDatabaseName("D:\\Desktop\\LearnC\\Qt\\My_netdisk\\TcpServer\\cloud.db");  //数据库路径名称
    if (m_db.open())                                                                   //数据库是否连接成功
    {
        // QMessageBox::critical(NULL, "打开数据库", "打开数据库成功");
        QSqlQuery query;
        query.exec("select * from usrInfo");  //查询现有用户
        while (query.next())
        {
            QString data = QString("%1,%2,%3")
                               .arg(query.value(0).toString())
                               .arg(query.value(1).toString())
                               .arg(query.value(2).toString());
            qDebug() << data;
        }
    }
    else
    {
        QMessageBox::critical(NULL, "打开数据库", "打开数据库失败");
    }
}
OpeDB::~OpeDB() { m_db.close(); }

bool OpeDB::handleRegist(const char* name, const char* pwd)  //处理注册请求
{
    if (name == NULL || pwd == NULL)  //判空
    {
        return false;
    }
    QString data = QString("insert into usrInfo(name,pwd) values(\'%1\',\'%2\')").arg(name).arg(pwd);  // 插入用户信息

    QSqlQuery query;
    return query.exec(data);  //执行语句
}

bool OpeDB::handleLogin(const char* name, const char* pwd)  //处理登录请求
{
    if (name == NULL || pwd == NULL)  //判空
    {
        return false;
    }
    QString data = QString("select * from usrInfo where name=\'%1\' and pwd = \'%2\' and online =0")
                       .arg(name)
                       .arg(pwd);  // 查找用户

    QSqlQuery query;
    query.exec(data);  //执行语句

    if (query.next())
    {
        data = QString("update usrInfo set online=1 where name=\'%1\' and pwd = \'%2\'")
                   .arg(name)
                   .arg(pwd);  // 修改在线状态

        // qDebug() << data;
        QSqlQuery query;
        query.exec(data);  //执行语句

        return true;
    }
    else
    {
        return false;
    }
}

void OpeDB::handleOffline(const char* name)  //处理用户下线
{
    if (name == NULL)  //判空
    {
        return;
    }
    QString data = QString("update usrInfo set online=0 where name=\'%1\'").arg(name);  // 用户下线

    QSqlQuery query;
    query.exec(data);  //执行语句
}

QStringList OpeDB::handleAllOnline()  //查找所有在线用户
{
    QString data = QString("select name from usrInfo where online=1");  // 查询在线

    QSqlQuery query;
    query.exec(data);  //执行语句
    QStringList result;
    result.clear();

    while (query.next()) { result.append(query.value(0).toString()); }

    return result;
}

int OpeDB::handleSearchUsr(const char* name)  //查找用户
{
    if (NULL == name)  //判空
    {
        return -1;
    }
    QString data = QString("select online from usrInfo where name=\'%1\'").arg(name);  //查找

    QSqlQuery query;
    query.exec(data);
    if (query.next())  //接收数据
    {
        int ret = query.value(0).toInt();
        if (1 == ret)  //在线
        {
            return 1;
        }
        else if (0 == ret)  //离线
        {
            return 0;
        }
    }
    else  //没找到
    {
        return -1;
    }
}

int OpeDB::handleAddFriend(const char* pername, const char* name)  //处理添加好友
{
    if (NULL == pername || NULL == name)  //判空
    {
        return -1;
    }
    QString data = QString("select * from friend where (id=(select id from usrInfo where name=\'%1\') and friendId = "
                           "(select id from usrInfo where name=\'%2\')) "
                           "or (id=(select id from usrInfo where name=\'%3\') and friendId = (select id from usrInfo "
                           "where name=\'%4\'))")
                       .arg(pername)
                       .arg(name)
                       .arg(name)
                       .arg(pername);  //查询是否已有好友

    // qDebug() << data;

    QSqlQuery query;
    query.exec(data);
    if (query.next())
    {
        return 0;  //双方已经是好友
    }
    else
    {
        data = QString("select online from usrInfo where name=\'%1\'").arg(pername);  //查对方是否在线
        QSqlQuery query;
        query.exec(data);
        if (query.next())
        {
            int ret = query.value(0).toInt();
            if (1 == ret)
            {
                return 1;  //在线
            }
            else if (0 == ret)
            {
                return 2;  //用户不在线
            }
        }
        else
        {
            return 3;  //用户不存在
        }
    }
}

void OpeDB::handleAgreeAddFriend(const char* pername, const char* name)  //同意加好友
{
    if (NULL == pername || NULL == name)  //判空
    {
        return;
    }
    QString data = QString("insert into friend(id, friendId) values((select id from usrInfo where name=\'%1\'), "
                           "(select id from usrInfo where name=\'%2\'))")  //插入好友信息
                       .arg(pername)
                       .arg(name);
    QSqlQuery query;
    query.exec(data);
}

QStringList OpeDB::handleFlushFriend(const char* name)  //处理刷新好友列表
{
    QStringList strFriendList;
    strFriendList.clear();

    if (NULL == name)  //判空
    {
        return strFriendList;
    }

    QString data = QString("select name from usrInfo where online=1 and id in (select id from friend where "
                           "friendId=(select id from usrInfo where name=\'%1\'))")
                       .arg(name);  //查询在线好友
    QSqlQuery query;
    query.exec(data);
    while (query.next())
    {
        strFriendList.append(query.value(0).toString());
        // qDebug() << "flush name:" << query.value(0).toString();
    }

    data = QString("select name from usrInfo where online=1 and id in (select friendId from friend where id=(select id "
                   "from usrInfo where name=\'%1\'))")  //查询在线好友
               .arg(name);

    query.exec(data);
    while (query.next())
    {
        strFriendList.append(query.value(0).toString());
        // qDebug() << "flush name:" << query.value(0).toString();
    }
    return strFriendList;
}

bool OpeDB::handleDelFriend(const char* name, const char* friendName)  //处理删除好友
{
    if (NULL == name || NULL == friendName)  //判空
    {
        return false;
    }
    QString data = QString("delete from friend where id=(select id from usrInfo where name=\'%1\') and "
                           "friendId=(select id from usrInfo where name=\'%2\')")
                       .arg(name)
                       .arg(friendName);  //删除
    QSqlQuery query;
    query.exec(data);

    if (query.next())
        return true;

    data = QString("delete from friend where id=(select id from usrInfo where name=\'%1\') and friendId=(select id "
                   "from usrInfo where name=\'%2\')")  //删除
               .arg(friendName)
               .arg(name);
    query.exec(data);

    if (query.next())
        return true;

    return false;
}

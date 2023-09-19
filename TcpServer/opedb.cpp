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

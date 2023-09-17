#include <QApplication>

#include "tcpsever.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    OpeDB::getInstance().init();  //连接数据库

    TcpSever w;
    w.show();

    return a.exec();
}

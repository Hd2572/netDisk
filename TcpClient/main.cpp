#include <QApplication>

//#include "friend.h"
//#include "online.h"
#include "opewidget.h"
#include "tcpclient.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    //    Online w;
    //    w.show();
    //    TcpClient w;
    //    w.show();
    OpeWidget w;
    w.show();
    //    Friend w;
    //    w.show();

    return a.exec();
}

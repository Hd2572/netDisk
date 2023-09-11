#include <QApplication>

#include "tcpsever.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    TcpSever w;
    w.show();

    return a.exec();
}

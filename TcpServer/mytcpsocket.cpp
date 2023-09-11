#include "mytcpsocket.h"

MyTcpSocket::MyTcpSocket()
{
    connect(this, SIGNAL(readyRead()), this, SLOT(recvMsg()));  //本身的readyread，本身的recvMsg处理
}

void MyTcpSocket::recvMsg()  //接收readyread
{
    // qDebug() << this->bytesAvailable();  //打印获得的数据大小
    uint uiPDULen = 0;
    this->read((char*)&uiPDULen, sizeof(uint));                      //全部大小
    uint uiMsgLen = uiPDULen - sizeof(PDU);                          //数据大小
    PDU* pdu = mkPDU(uiMsgLen);                                      //创建新pdu
    this->read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));  //读数据
    qDebug() << pdu->uiMsgType << (char*)(pdu->caMsg);
}

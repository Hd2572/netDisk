#include "tcpclient.h"

#include "privatechat.h"
#include "ui_tcpclient.h"

TcpClient::TcpClient(QWidget* parent) : QWidget(parent), ui(new Ui::TcpClient)  //构造
{
    ui->setupUi(this);

    resize(500, 250);  //重设窗口大小
    loadConfig();      //加载配置文件

    connect(&m_tcpSocket, SIGNAL(connected()), this, SLOT(showConnect()));  //连接连接成功信号与弹窗槽
    connect(&m_tcpSocket, SIGNAL(readyRead()), this, SLOT(recvMsg()));      //连接接收服务器信息槽

    m_tcpSocket.connectToHost(QHostAddress(m_strIP), m_usport);  //连接服务器，成功则发送connect信号
}

TcpClient::~TcpClient() { delete ui; }

void TcpClient::loadConfig()  //加载配置文件
{
    QFile file(":/client.config");       //打开配置文件
    if (file.open(QIODevice::ReadOnly))  //只读
    {
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();  //转为string保存

        file.close();

        strData.replace("\r\n", " ");
        QStringList strList = strData.split(" ");  //空格分割赋值给ip和端口

        m_strIP = strList.at(0);
        m_usport = strList.at(1).toUShort();
        // qDebug() << "ip:" << m_strIP << "port:" << m_usport;
    }
    else
    {
        QMessageBox::critical(this, "open config", "open config failed");
    }
}

TcpClient& TcpClient::getInstance()  //单例
{
    static TcpClient instance;
    return instance;
}

QTcpSocket& TcpClient::getTcpSocket()  //返回socket
{
    return m_tcpSocket;
}

QString TcpClient::loginName()  //返回当前登录用户名
{
    return m_strLoginName;
}

QString TcpClient::curPath()  //返回当前路径
{
    return m_strCurPath;
}

void TcpClient::setCurPath(QString strCurPath)  //设置当前路径
{
    m_strCurPath = strCurPath;
}

void TcpClient::showConnect()  //连接成功弹窗
{
    QMessageBox::information(this, "连接服务器", "连接服务器成功");
}

void TcpClient::recvMsg()  //接收数据
{
    if (!OpeWidget::getInstance().getBook()->getDownloadStatus())  //不在下载状态
    {
        uint uiPDULen = 0;
        m_tcpSocket.read((char*)&uiPDULen, sizeof(uint));                      //全部大小
        uint uiMsgLen = uiPDULen - sizeof(PDU);                                //数据大小
        PDU* pdu = mkPDU(uiMsgLen);                                            //创建新pdu
        m_tcpSocket.read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));  //读数据

        // qDebug() << pdu->uiMsgType << endl;

        switch (pdu->uiMsgType)  //判断消息类型
        {
            case ENUM_MSG_TYPE_REGIST_RESPOND:  //注册回复
            {
                if (strcmp(pdu->caData, REGIST_OK) == 0)  //弹窗
                {
                    QMessageBox::information(this, "注册", REGIST_OK);
                }
                else if (strcmp(pdu->caData, REGIST_FAILED) == 0)
                {
                    QMessageBox::warning(this, "注册", REGIST_FAILED);
                }

                break;
            }
            case ENUM_MSG_TYPE_LOGIN_RESPOND:  //登录回复
            {
                if (strcmp(pdu->caData, LOGIN_OK) == 0)  //弹窗
                {
                    m_strCurPath = QString("./%1").arg(m_strLoginName);  //当前目录为用户根目录
                    QMessageBox::information(this, "登录", LOGIN_OK);
                    OpeWidget::getInstance().show();  //跳转登录成功页面
                    this->hide();                     //隐藏登录界面
                }
                else if (strcmp(pdu->caData, LOGIN_FAILED) == 0)
                {
                    QMessageBox::warning(this, "登录", LOGIN_FAILED);
                }

                break;
            }
            case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND:  //在线用户回复
            {
                // qDebug() << 1111;
                OpeWidget::getInstance().getFriend()->showAllOnlineUsr(pdu);  //传入信息
                break;
            }
            case ENUM_MSG_TYPE_SEARCH_USR_RESPOND:  //搜索用户
            {
                if (0 == strcmp(SEARCH_USR_NO, pdu->caData))  //用户不存在
                {
                    QMessageBox::information(
                        this, "搜索",
                        QString("%1: not exist").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
                }
                else if (0 == strcmp(SEARCH_USR_ONLINE, pdu->caData))  //在线
                {
                    QMessageBox::information(
                        this, "搜索", QString("%1: online").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
                }
                else if (0 == strcmp(SEARCH_USR_OFFLINE, pdu->caData))  //离线
                {
                    QMessageBox::information(
                        this, "搜索",
                        QString("%1: offline").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
                }
                break;
            }
            case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:  //加好友申请
            {
                char caName[32] = {'\0'};
                strncpy(caName, pdu->caData + 32, 32);  //申请方用户名
                // qDebug() << "申请方：" << caName << endl;
                int ret =
                    QMessageBox::information(this, "添加好友", QString("%1 want to add you as friend ?").arg(caName),
                                             QMessageBox::Yes, QMessageBox::No);  //弹出提示框

                PDU* respdu = mkPDU(0);                   //回复pdu
                memcpy(respdu->caData, pdu->caData, 64);  //申请方回复方用户名
                if (QMessageBox::Yes == ret)              //同意
                {
                    respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGGREE;  //同意加好友
                }
                else
                {
                    respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;  //拒绝加好友
                }
                m_tcpSocket.write((char*)respdu, respdu->uiPDULen);  //发给服务器
                free(respdu);
                respdu = NULL;
                break;
            }
            case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:  //加好友回复
            {
                QMessageBox::information(this, "添加好友", pdu->caData);  //提示框
                break;
            }
            case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE:  //对方同意加好友
            {
                char caPerName[32] = {'\0'};
                memcpy(caPerName, pdu->caData, 32);                                                    //对方名
                QMessageBox::information(this, "添加好友", QString("添加%1好友成功").arg(caPerName));  //提示框
                break;
            }
            case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:  //对方拒绝加好友
            {
                char caPerName[32] = {'\0'};
                memcpy(caPerName, pdu->caData, 32);
                QMessageBox::information(this, "添加好友", QString("添加%1好友失败").arg(caPerName));
                break;
            }
            case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:  //刷新好友
            {
                OpeWidget::getInstance().getFriend()->updateFriendList(pdu);  //传入pdu，刷新
                break;
            }
            case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:  //被删除好友通知
            {
                char caName[32] = {'\0'};
                memcpy(caName, pdu->caData, 32);  //对方名字
                QMessageBox::information(this, "删除好友", QString("%1 删除你作为他的好友").arg(caName));
                break;
            }
            case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND:  //删除好友回复
            {
                QMessageBox::information(this, "删除好友", "删除好友成功");
                break;
            }
            case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:  //好友私聊
            {
                if (PrivateChat::getInstance().isHidden())  //显示私聊窗口
                {
                    PrivateChat::getInstance().show();
                }
                char caSendName[32] = {'\0'};
                memcpy(caSendName, pdu->caData, 32);  //对方名
                QString strSendName = caSendName;
                PrivateChat::getInstance().setChatName(strSendName);  //设置对方名

                PrivateChat::getInstance().updateMsg(pdu);  //更新

                break;
            }
            case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:  //群聊
            {
                OpeWidget::getInstance().getFriend()->updateGroupMsg(pdu);  //更新消息
                break;
            }
            case ENUM_MSG_TYPE_CREATE_DIR_RESPOND:  //创建文件夹回复
            {
                QMessageBox::information(this, "创建文件夹", pdu->caData);  //消息弹窗
                break;
            }
            case ENUM_MSG_TYPE_FLUSH_FILE_RESPOND:  //刷新文件回复
            {
                OpeWidget::getInstance().getBook()->updateFileList(pdu);               //更新文件列表
                QString strEnterDir = OpeWidget::getInstance().getBook()->enterDir();  //进入的路径

                if (!strEnterDir.isEmpty())  //不是空
                {
                    m_strCurPath = m_strCurPath + "/" + strEnterDir;  //更新当前路径
                    // qDebug() << "enter dir:" << m_strCurPath;
                }
                OpeWidget::getInstance().getBook()->clearEnterDir();  //清空保存的进入
                break;
            }
            case ENUM_MSG_TYPE_DEL_DIR_RESPOND:  //删除文件夹回复
            {
                QMessageBox::information(this, "删除文件夹", pdu->caData);  //删除文件夹回复
                break;
            }
            case ENUM_MSG_TYPE_RENAME_FILE_RESPOND:  //重命名文件回复
            {
                QMessageBox::information(this, "重命名文件", pdu->caData);  //重命名文件回复
                break;
            }
            case ENUM_MSG_TYPE_ENTER_DIR_RESPOND:  //进入文件夹回复
            {
                OpeWidget::getInstance().getBook()->clearEnterDir();  //清空进入的目录
                QMessageBox::information(this, "进入文件夹", pdu->caData);
                break;
            }
            case ENUM_MSG_TYPE_DEL_FILE_RESPOND:  //删除常规文件回复
            {
                QMessageBox::information(this, "删除文件", pdu->caData);
                break;
            }
            case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND:  //上传文件回复
            {
                QMessageBox::information(this, "上传文件", pdu->caData);
                break;
            }
            case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND:  //下载文件回复
            {
                // qDebug() << pdu->caData;
                char caFileName[32] = {'\0'};
                sscanf(pdu->caData, "%s %lld", caFileName,
                       &(OpeWidget::getInstance().getBook()->m_iTotal));                         //文件名和大小
                if (strlen(caFileName) > 0 && OpeWidget::getInstance().getBook()->m_iTotal > 0)  //判空
                {
                    OpeWidget::getInstance().getBook()->setDownloadStatus(true);                //设置为下载状态
                    m_file.setFileName(OpeWidget::getInstance().getBook()->getSaveFilePath());  //设置保存路径
                    if (!m_file.open(QIODevice::WriteOnly))                                     //打开失败
                    {
                        QMessageBox::warning(this, "下载文件", "获得保存文件的路径失败");
                    }
                }
                break;
            }
            case ENUM_MSG_TYPE_SHARE_FILE_RESPOND:  //分享文件回复
            {
                QMessageBox::information(this, "共享文件", pdu->caData);
                break;
            }
            case ENUM_MSG_TYPE_SHARE_FILE_NOTE:  //分享文件请求
            {
                char* pPath = new char[pdu->uiMsgLen];
                memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);  //文件路径

                char* pos = strrchr(pPath, '/');  //找最后一个'/'
                if (NULL != pos)                  //判空
                {
                    pos++;
                    QString strNote = QString("%1 share file->%2 \n Do you accept ?").arg(pdu->caData).arg(pos);  //通知
                    int ret = QMessageBox::question(this, "共享文件", strNote);  //窗口
                    if (QMessageBox::Yes == ret)                                 //同意
                    {
                        PDU* respdu = mkPDU(pdu->uiMsgLen);
                        respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND;  //分享请求回复
                        memcpy(respdu->caMsg, pdu->caMsg, pdu->uiMsgLen);           //文件路径
                        QString strName = TcpClient::getInstance().loginName();     //获取自己用户名
                        strcpy(respdu->caData, strName.toStdString().c_str());
                        m_tcpSocket.write((char*)respdu, respdu->uiPDULen);  //发送
                    }
                }
                delete[] pPath;
                break;
            }
            case ENUM_MSG_TYPE_MOVE_FILE_RESPOND:  //移动文件回复
            {
                QMessageBox::information(this, "移动文件", pdu->caData);
            }
            default: break;
        }

        free(pdu);
        pdu = NULL;
    }
    else  //在下载状态
    {
        QByteArray buffer = m_tcpSocket.readAll();  //读数据
        m_file.write(buffer);                       //写入文件
        Book* pBook = OpeWidget::getInstance().getBook();
        pBook->m_iRecved += buffer.size();  //更新已接收部分

        if (pBook->m_iTotal == pBook->m_iRecved)  //接收完了
        {
            m_file.close();       //关闭文件
            pBook->m_iTotal = 0;  //置0
            pBook->m_iRecved = 0;
            pBook->setDownloadStatus(false);                             //不在下载状态
            QMessageBox::information(this, "下载文件", "下载文件成功");  //提示
        }
        else if (pBook->m_iTotal < pBook->m_iRecved)  //接多了
        {
            m_file.close();  //关文件，置0
            pBook->m_iTotal = 0;
            pBook->m_iRecved = 0;
            pBook->setDownloadStatus(false);  //不在下载状态

            QMessageBox::critical(this, "下载文件", "下载文件失败");  //提示失败
        }
    }
}

#if 0  //测试函数注释掉
void TcpClient::on_send_pd_clicked()  //发送按钮槽
{
    QString strMsg = ui->lineEdit->text();  //获取输入框数据
    if (!strMsg.isEmpty())
    {
        PDU* pdu = mkPDU(strMsg.toUtf8().size() + 1);  //申请地址
        pdu->uiMsgType = 8888;
        memcpy(pdu->caMsg, strMsg.toStdString().c_str(), strMsg.toUtf8().size());  //拷贝
        // qDebug() << pdu->uiPDULen << (char*)(pdu->caMsg);
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
        free(pdu);  //释放空间
        pdu = NULL;
    }
    else
    {
        QMessageBox::warning(this, "信息发生", "发送的消息不能为空");
    }
}
#endif
void TcpClient::on_login_pb_clicked()  //登录按钮
{
    QString strName = ui->name_le->text();  //获取用户名密码
    QString strPwd = ui->pwd_le->text();

    if (!strName.isEmpty() && !strPwd.isEmpty())
    {
        m_strLoginName = strName;  //保存我的用户名
        PDU* pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;             //消息类型：登录请求
        strncpy(pdu->caData, strName.toStdString().c_str(), 32);  //将用户名密码存到caData
        strncpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);

        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);  // socket发送
        free(pdu);                                     //释放空间
        pdu = NULL;
    }
    else
    {
        QMessageBox::critical(this, "登录", "登录失败:用户名或密码为空");
    }
}

void TcpClient::on_regist_pb_clicked()  //注册按钮
{
    QString strName = ui->name_le->text();  //获取用户名密码

    QString strPwd = ui->pwd_le->text();
    if (!strName.isEmpty() && !strPwd.isEmpty())
    {
        m_strLoginName = strName;  //保存登录用户名
        PDU* pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST;            //消息类型：注册请求
        strncpy(pdu->caData, strName.toStdString().c_str(), 32);  //将用户名密码存到caData
        strncpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);

        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);  // socket发送
        free(pdu);                                     //释放空间
        pdu = NULL;
    }
    else
    {
        QMessageBox::critical(this, "注册", "注册失败:用户名或密码为空");
    }
}

void TcpClient::on_cancel_pb_clicked()  //注销按钮
{
}

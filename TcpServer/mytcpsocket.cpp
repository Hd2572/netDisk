#include "mytcpsocket.h"

#include "mytcpserver.h"

MyTcpSocket::MyTcpSocket()
{
    connect(this, SIGNAL(readyRead()), this, SLOT(recvMsg()));           //本身的readyread，本身的recvMsg处理
    connect(this, SIGNAL(disconnected()), this, SLOT(clientOffline()));  //关联客户端下线槽

    m_bUpload = false;  //初始不在上传文件状态
}

QString MyTcpSocket::getName()  //获取用户名
{
    return m_strName;
}

void MyTcpSocket::recvMsg()  //接收readyread
{
    // qDebug() << this->bytesAvailable();  //打印获得的数据大小
    if (!m_bUpload)  //没在上传文件状态
    {
        uint uiPDULen = 0;
        this->read((char*)&uiPDULen, sizeof(uint));                      //全部大小
        uint uiMsgLen = uiPDULen - sizeof(PDU);                          //数据大小
        PDU* pdu = mkPDU(uiMsgLen);                                      //创建新pdu
        this->read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));  //读数据

        switch (pdu->uiMsgType)  //判断消息类型
        {
            case ENUM_MSG_TYPE_REGIST_REQUEST:  //注册请求
            {
                char caName[32] = {'\0'};
                char caPwd[32] = {'\0'};
                strncpy(caName, pdu->caData, 32);
                strncpy(caPwd, pdu->caData + 32, 32);  //获取用户名密码

                // qDebug() << caName << caPwd << pdu->uiMsgType;

                bool ret = OpeDB::getInstance().handleRegist(caName, caPwd);  //数据库处理

                PDU* respdu = mkPDU(0);                            //回复pdu
                respdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;  //注册回复

                if (ret)  //成功
                {
                    strcpy(respdu->caData, REGIST_OK);
                    QDir dir;
                    qDebug() << "create dir: " << dir.mkdir(QString("./%1").arg(caName));  //为新用户创建文件夹
                }
                else  //失败
                {
                    strcpy(respdu->caData, REGIST_FAILED);
                }

                write((char*)respdu, respdu->uiPDULen);  // socket发送
                free(respdu);                            //释放空间
                respdu = NULL;

                break;
            }
            case ENUM_MSG_TYPE_LOGIN_REQUEST:  //登录请求
            {
                char caName[32] = {'\0'};
                char caPwd[32] = {'\0'};
                strncpy(caName, pdu->caData, 32);
                strncpy(caPwd, pdu->caData + 32, 32);  //获取用户名密码

                bool ret = OpeDB::getInstance().handleLogin(caName, caPwd);  //登录

                PDU* respdu = mkPDU(0);                           //回复pdu
                respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;  //登录回复

                if (ret)  //成功
                {
                    strcpy(respdu->caData, LOGIN_OK);
                    m_strName = caName;  //保存用户名
                }
                else  //失败
                {
                    strcpy(respdu->caData, LOGIN_FAILED);
                }

                write((char*)respdu, respdu->uiPDULen);  // socket发送
                free(respdu);                            //释放空间
                respdu = NULL;
                break;
            }
            case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:  //查在线用户请求
            {
                QStringList ret = OpeDB::getInstance().handleAllOnline();  //查询
                uint uiMsgLen = ret.size() * 32;

                // qDebug() << "请求：" << pdu->uiMsgType;

                PDU* respdu = mkPDU(uiMsgLen);  //回复pdu
                respdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
                for (int i = 0; i < ret.size(); i++)  //拷贝
                {
                    memcpy((char*)respdu->caMsg + i * 32, ret.at(i).toStdString().c_str(), ret.at(i).size());
                }
                write((char*)respdu, respdu->uiPDULen);  // socket发送

                // qDebug() << "回复：" << respdu->uiMsgType;

                free(respdu);  //释放空间
                respdu = NULL;

                break;
            }
            case ENUM_MSG_TYPE_SEARCH_USR_REQUEST:  //查找用户请求
            {
                int ret = OpeDB::getInstance().handleSearchUsr(pdu->caData);  //查找
                PDU* respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_RESPOND;
                if (-1 == ret)  //没找到
                {
                    strcpy(respdu->caData, SEARCH_USR_NO);
                }
                else if (1 == ret)  //在线
                {
                    strcpy(respdu->caData, SEARCH_USR_ONLINE);
                }
                else if (0 == ret)  //离线
                {
                    strcpy(respdu->caData, SEARCH_USR_OFFLINE);
                }
                write((char*)respdu, respdu->uiPDULen);  //发送
                free(respdu);
                respdu = NULL;
                break;
            }
            case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:  //添加好友请求
            {
                char caPerName[32] = {'\0'};  //目标用户名
                char caName[32] = {'\0'};     //申请方用户名
                strncpy(caPerName, pdu->caData, 32);
                strncpy(caName, pdu->caData + 32, 32);

                int ret = OpeDB::getInstance().handleAddFriend(caPerName, caName);  //数据库处理

                PDU* respdu = NULL;  //回复pdu
                if (-1 == ret)       //未知错误
                {
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                    strcpy(respdu->caData, UNKNOW_ERROR);
                    write((char*)respdu, respdu->uiPDULen);
                    free(respdu);
                    respdu = NULL;
                }
                else if (0 == ret)  //已经是好友
                {
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                    strcpy(respdu->caData, EXISTED_FRIEND);
                    write((char*)respdu, respdu->uiPDULen);
                    free(respdu);
                    respdu = NULL;
                }
                else if (1 == ret)  //对方在线
                {
                    // qDebug() << "申请方：" << caName << "对方" << caPerName << endl;
                    MyTcpServer::getInstance().resend(caPerName, pdu);  //好友申请转发给对方
                }
                else if (2 == ret)  //对方不在线
                {
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                    strcpy(respdu->caData, ADD_FRIEND_OFFLINE);
                    write((char*)respdu, respdu->uiPDULen);
                    free(respdu);
                    respdu = NULL;
                }
                else if (3 == ret)  //对方用户名不存在
                {
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                    strcpy(respdu->caData, ADD_FRIEND_NO_EXIST);
                    write((char*)respdu, respdu->uiPDULen);
                    free(respdu);
                    respdu = NULL;
                }
                break;
            }
            case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE:  //同意加好友
            {
                char caPerName[32] = {'\0'};
                char caName[32] = {'\0'};
                strncpy(caPerName, pdu->caData, 32);                           //对方名
                strncpy(caName, pdu->caData + 32, 32);                         //申请人名
                OpeDB::getInstance().handleAgreeAddFriend(caPerName, caName);  //数据库操作

                MyTcpServer::getInstance().resend(caName, pdu);  //转发给申请人
                break;
            }
            case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:  //拒绝加好友
            {
                char caName[32] = {'\0'};
                strncpy(caName, pdu->caData + 32, 32);           //申请人名
                MyTcpServer::getInstance().resend(caName, pdu);  //转发给申请人
                break;
            }
            case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:  //刷新好友列表
            {
                char caName[32] = {'\0'};
                strncpy(caName, pdu->caData, 32);  //请求方用户名

                QStringList ret = OpeDB::getInstance().handleFlushFriend(caName);  //数据库查询，返回好友列表

                uint uiMsgLen = ret.size() * 32;  //回复pdu大小
                PDU* respdu = mkPDU(uiMsgLen);

                respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;

                for (int i = 0; i < ret.size(); i++)  //复制好友用户名
                {
                    memcpy((char*)(respdu->caMsg) + i * 32, ret.at(i).toStdString().c_str(), ret.at(i).size());
                }

                write((char*)respdu, respdu->uiPDULen);  //发给客户端
                free(respdu);
                respdu = NULL;
                break;
            }
            case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:  //删除好友请求
            {
                char caSelfName[32] = {'\0'};
                char caFriendName[32] = {'\0'};

                strncpy(caSelfName, pdu->caData, 32);                            //申请人用户名
                strncpy(caFriendName, pdu->caData + 32, 32);                     //要删除的好友
                OpeDB::getInstance().handleDelFriend(caSelfName, caFriendName);  //数据库处理

                PDU* respdu = mkPDU(0);  // pdu
                respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
                strcpy(respdu->caData, DEL_FRIEND_OK);

                write((char*)respdu, respdu->uiPDULen);  //发送
                free(respdu);
                respdu = NULL;

                MyTcpServer::getInstance().resend(caFriendName, pdu);  //转发给被删除方，没在线则找不到不转发

                break;
            }
            case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:  //好友私聊请求
            {
                char caPerName[32] = {'\0'};
                memcpy(caPerName, pdu->caData + 32, 32);  //目标方名
                char caName[32] = {'\0'};
                memcpy(caName, pdu->caData, 32);  //发起方名
                // qDebug() << caName << "-->" << caPerName << (char*)(pdu->caMsg);
                MyTcpServer::getInstance().resend(caPerName, pdu);  //如果在线就转发给对方

                break;
            }
            case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:  //群聊消息
            {
                char caName[32] = {'\0'};
                strncpy(caName, pdu->caData, 32);  //发送方名字

                QStringList onlineFriend = OpeDB::getInstance().handleFlushFriend(caName);  //获得好友列表

                QString tmp;
                for (int i = 0; i < onlineFriend.size(); i++)  //挨个好友转发
                {
                    tmp = onlineFriend.at(i);
                    MyTcpServer::getInstance().resend(tmp.toStdString().c_str(), pdu);
                }
                break;
            }
            case ENUM_MSG_TYPE_CREATE_DIR_REQUEST:  //创建文件夹请求
            {
                QDir dir;
                QString strCurPath = QString("%1").arg((char*)(pdu->caMsg));  //当前目录
                // qDebug() << strCurPath;
                bool ret = dir.exists(strCurPath);  //是否存在
                PDU* respdu = NULL;

                if (ret)  //当前目录存在
                {
                    char caNewDir[32] = {'\0'};
                    memcpy(caNewDir, pdu->caData + 32, 32);
                    QString strNewPath = strCurPath + "/" + caNewDir;  //新建文件夹
                    // qDebug() << strNewPath;
                    ret = dir.exists(strNewPath);
                    // qDebug() << "-->" << ret;

                    if (ret)  //创建的文件夹重名
                    {
                        respdu = mkPDU(0);
                        respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                        strcpy(respdu->caData, FILE_NAME_EXIST);
                    }
                    else  //创建的文件夹不存在
                    {
                        dir.mkdir(strNewPath);  //创建新文件夹
                        respdu = mkPDU(0);
                        respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                        strcpy(respdu->caData, CREAT_DIR_OK);
                    }
                }
                else  //当前目录不存在
                {
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                    strcpy(respdu->caData, DIR_NO_EXIST);
                }
                write((char*)respdu, respdu->uiPDULen);  //回复
                free(respdu);
                respdu = NULL;
                break;
            }
            case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST:  //刷新文件请求
            {
                char* pCurPath = new char[pdu->uiMsgLen];  //当前目录
                memcpy(pCurPath, pdu->caMsg, pdu->uiMsgLen);

                QDir dir(pCurPath);  //进入目录

                QFileInfoList fileInfoList = dir.entryInfoList();  //文件夹内文件列表
                int iFileCount = fileInfoList.size();              //文件数

                PDU* respdu = mkPDU(sizeof(FileInfo) * iFileCount);  // pdu

                respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
                FileInfo* pFileInfo = NULL;
                QString strFileName;

                for (int i = 0; i < iFileCount; i++)
                {
                    pFileInfo = (FileInfo*)(respdu->caMsg) + i;

                    strFileName = fileInfoList[i].fileName();  //文件名

                    memcpy(pFileInfo->caFileName, strFileName.toStdString().c_str(),
                           strFileName.toUtf8().size());  //写入pdu

                    if (fileInfoList[i].isDir())  //是文件夹
                    {
                        pFileInfo->iFileType = 0;
                    }
                    else if (fileInfoList[i].isFile())  //是文件
                    {
                        pFileInfo->iFileType = 1;
                    }
                }
                write((char*)respdu, respdu->uiPDULen);  //发送
                free(respdu);
                respdu = NULL;

                delete[] pCurPath;
                pCurPath = NULL;

                break;
            }
            case ENUM_MSG_TYPE_DEL_DIR_REQUEST:  //删除文件夹请求
            {
                char caName[32] = {'\0'};
                strcpy(caName, pdu->caData);  //删除文件夹名

                char* pPath = new char[pdu->uiMsgLen];
                memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
                QString strPath = QString("%1/%2").arg(pPath).arg(caName);  //删除文件夹路径
                // qDebug() << strPath;
                delete[] pPath;
                pPath = NULL;

                QFileInfo fileInfo(strPath);  //创建文件对象
                bool ret = false;

                if (fileInfo.isDir())  //是文件夹
                {
                    QDir dir;
                    dir.setPath(strPath);
                    ret = dir.removeRecursively();  //递归删除
                }
                else if (fileInfo.isFile())  //常规文件
                {
                    ret = false;
                }
                PDU* respdu = NULL;

                if (ret)  //删除成功
                {
                    respdu = mkPDU(strlen(DEL_DIR_OK) + 1);
                    respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;
                    memcpy(respdu->caData, DEL_DIR_OK, strlen(DEL_DIR_OK));
                }
                else  //失败
                {
                    respdu = mkPDU(strlen(DEL_DIR_FAILURED) + 1);
                    respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;
                    memcpy(respdu->caData, DEL_DIR_FAILURED, strlen(DEL_DIR_FAILURED));
                }

                write((char*)respdu, respdu->uiPDULen);  //发送
                free(respdu);
                respdu = NULL;

                break;
            }
            case ENUM_MSG_TYPE_RENAME_FILE_REQUEST:  //重命名请求
            {
                char caOldName[32] = {'\0'};
                char caNewName[32] = {'\0'};
                strncpy(caOldName, pdu->caData, 32);       //旧名字
                strncpy(caNewName, pdu->caData + 32, 32);  //新名字

                char* pPath = new char[pdu->uiMsgLen];
                memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);  //当前路径

                QString strOldPath = QString("%1/%2").arg(pPath).arg(caOldName);  //旧路径
                QString strNewPath = QString("%1/%2").arg(pPath).arg(caNewName);  //新路径

                //            qDebug() << strOldPath;
                //            qDebug() << strNewPath;
                delete[] pPath;
                pPath = NULL;

                QDir dir;
                bool ret = dir.rename(strOldPath, strNewPath);  //改名

                PDU* respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_RESPOND;
                if (ret)  //改成功
                {
                    strcpy(respdu->caData, RENAME_FILE_OK);
                }
                else  //失败，重复名
                {
                    strcpy(respdu->caData, RENAME_FILE_FAILURED);
                }

                write((char*)respdu, respdu->uiPDULen);  //发送
                free(respdu);
                respdu = NULL;

                break;
            }
            case ENUM_MSG_TYPE_ENTER_DIR_REQUEST:  //进入文件夹请求
            {
                char caEnterName[32] = {'\0'};
                strncpy(caEnterName, pdu->caData, 32);  //进入目录名

                char* pPath = new char[pdu->uiMsgLen];
                memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);  //当前目录

                QString strPath = QString("%1/%2").arg(pPath).arg(caEnterName);  //进入目录

                // qDebug() << strPath;
                delete[] pPath;
                pPath = NULL;

                QFileInfo fileInfo(strPath);  //文件对象
                PDU* respdu = NULL;
                if (fileInfo.isDir())  //是文件夹
                {
                    QDir dir(strPath);                                 // 进入
                    QFileInfoList fileInfoList = dir.entryInfoList();  //文件列表

                    int iFileCount = fileInfoList.size();  //多少文件

                    respdu = mkPDU(sizeof(FileInfo) * iFileCount);
                    respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;  //刷新回复，下与刷新同
                    FileInfo* pFileInfo = NULL;

                    QString strFileName;
                    for (int i = 0; i < iFileCount; i++)
                    {
                        pFileInfo = (FileInfo*)(respdu->caMsg) + i;
                        strFileName = fileInfoList[i].fileName();

                        memcpy(pFileInfo->caFileName, strFileName.toStdString().c_str(), strFileName.toUtf8().size());
                        if (fileInfoList[i].isDir())
                        {
                            pFileInfo->iFileType = 0;
                        }
                        else if (fileInfoList[i].isFile())
                        {
                            pFileInfo->iFileType = 1;
                        }
                    }
                    write((char*)respdu, respdu->uiPDULen);  //发送
                    free(respdu);
                    respdu = NULL;
                }
                else if (fileInfo.isFile())  //是常规文件
                {
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_RESPOND;  //进入回复
                    strcpy(respdu->caData, ENTER_DIR_FAILURED);

                    write((char*)respdu, respdu->uiPDULen);
                    free(respdu);
                    respdu = NULL;
                }

                break;
            }
            case ENUM_MSG_TYPE_DEL_FILE_REQUEST:
            {
                char caName[32] = {'\0'};
                strcpy(caName, pdu->caData);  //删除文件名

                char* pPath = new char[pdu->uiMsgLen];
                memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
                QString strPath = QString("%1/%2").arg(pPath).arg(caName);  //删除文件路径
                // qDebug() << strPath;
                delete[] pPath;
                pPath = NULL;

                QFileInfo fileInfo(strPath);  //创建文件对象
                bool ret = false;

                if (fileInfo.isDir())  //是文件夹
                {
                    ret = false;
                }
                else if (fileInfo.isFile())  //常规文件
                {
                    QDir dir;
                    ret = dir.remove(strPath);  //删除
                }
                PDU* respdu = NULL;

                if (ret)  //删除成功
                {
                    respdu = mkPDU(strlen(DEL_FILE_OK) + 1);
                    respdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_RESPOND;
                    memcpy(respdu->caData, DEL_FILE_OK, strlen(DEL_FILE_OK));
                }
                else  //失败
                {
                    respdu = mkPDU(strlen(DEL_FILE_FAILURED) + 1);
                    respdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_RESPOND;
                    memcpy(respdu->caData, DEL_FILE_FAILURED, strlen(DEL_FILE_FAILURED));
                }

                write((char*)respdu, respdu->uiPDULen);  //发送
                free(respdu);
                respdu = NULL;

                break;
            }
            case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST:  //上传文件请求
            {
                char caFileName[32] = {'\0'};
                qint64 fileSize = 0;
                sscanf(pdu->caData, "%s %lld", caFileName, &fileSize);  //获取文件名和文件大小

                char* pPath = new char[pdu->uiMsgLen];  //当前路径
                memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
                QString strPath = QString("%1/%2").arg(pPath).arg(caFileName);  //文件路径

                // qDebug() << strPath;
                delete[] pPath;
                pPath = NULL;

                m_file.setFileName(strPath);  //文件

                if (m_file.open(QIODevice::WriteOnly))  //以只写的方式打开文件，若文件不存在，则会自动创建文件
                {
                    m_bUpload = true;     //开始连接
                    m_iTotal = fileSize;  //文件大小
                    m_iRecved = 0;        //已发送0
                }

                break;
            }
            default: break;
        }

        free(pdu);
        pdu = NULL;
    }
    else  //在传输文件状态
    {
        PDU* respdu = NULL;
        respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;  //上传文件回复

        QByteArray buff = readAll();  //接收发过来的数据
        m_file.write(buff);           //写入文件中
        m_iRecved += buff.size();     //接受了多少

        if (m_iTotal == m_iRecved)  //接收完了
        {
            m_file.close();     //关闭文件
            m_bUpload = false;  //设为没在上传状态

            strcpy(respdu->caData, UPLOAD_FILE_OK);  //回复上传成功
            write((char*)respdu, respdu->uiPDULen);  //发送
            free(respdu);
            respdu = NULL;
        }
        else if (m_iTotal < m_iRecved)  //接收多了
        {
            m_file.close();                                //关闭文件
            m_bUpload = false;                             //设为没在上传
            strcpy(respdu->caData, UPLOAD_FILE_FAILURED);  //回复失败

            write((char*)respdu, respdu->uiPDULen);  //发送
            free(respdu);
            respdu = NULL;
        }
    }
    // qDebug() << caName << caPwd << pdu->uiMsgType;
}

void MyTcpSocket::clientOffline()  //客户端下线
{
    OpeDB::getInstance().handleOffline(m_strName.toStdString().c_str());  //更改在线状态
    emit offline(this);                                                   //发送下线信号
}

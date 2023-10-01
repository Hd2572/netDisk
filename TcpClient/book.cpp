#include "book.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "opewidget.h"
#include "sharefile.h"
#include "tcpclient.h"

Book::Book(QWidget* parent) : QWidget(parent)
{
    m_strEnterDir.clear();  //清空进入的路径

    m_bDownload = false;  //初始不在下载状态

    m_pTimer = new QTimer;  //计时器

    m_pBookListW = new QListWidget;  //设置文件列表和按钮
    m_pReturnPB = new QPushButton("返回");
    m_pCreateDirPB = new QPushButton("创建文件夹");
    m_pDelDirPB = new QPushButton("删除文件夹");
    m_pRenamePB = new QPushButton("重命名文件");
    m_pFlushFilePB = new QPushButton("刷新文件");

    QVBoxLayout* pDirVBL = new QVBoxLayout;  //垂直布局
    pDirVBL->addWidget(m_pReturnPB);
    pDirVBL->addWidget(m_pCreateDirPB);
    pDirVBL->addWidget(m_pDelDirPB);
    pDirVBL->addWidget(m_pRenamePB);
    pDirVBL->addWidget(m_pFlushFilePB);

    m_pUploadPB = new QPushButton("上传文件");
    m_pDownLoadPB = new QPushButton("下载文件");
    m_pDelFilePB = new QPushButton("删除文件");
    m_pShareFilePB = new QPushButton("共享文件");
    m_pMoveFilePB = new QPushButton("移动文件");
    m_pSelectDirPB = new QPushButton("目标目录");
    m_pSelectDirPB->setEnabled(false);  //不可选

    QVBoxLayout* pFileVBL = new QVBoxLayout;  //垂直布局
    pFileVBL->addWidget(m_pUploadPB);
    pFileVBL->addWidget(m_pDownLoadPB);
    pFileVBL->addWidget(m_pDelFilePB);
    pFileVBL->addWidget(m_pShareFilePB);
    pFileVBL->addWidget(m_pMoveFilePB);
    pFileVBL->addWidget(m_pSelectDirPB);

    QHBoxLayout* pMain = new QHBoxLayout;  //水平布局
    pMain->addWidget(m_pBookListW);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);

    setLayout(pMain);

    connect(m_pCreateDirPB, SIGNAL(clicked(bool)), this, SLOT(createDir()));                       //创建文件夹
    connect(m_pFlushFilePB, SIGNAL(clicked(bool)), this, SLOT(flushFile()));                       //刷新文件列表
    connect(m_pDelDirPB, SIGNAL(clicked(bool)), this, SLOT(delDir()));                             //删除文件夹
    connect(m_pRenamePB, SIGNAL(clicked(bool)), this, SLOT(renameFile()));                         //重命名文件
    connect(m_pBookListW, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(enterDir(QModelIndex)));  //进入文件夹
    connect(m_pReturnPB, SIGNAL(clicked(bool)), this, SLOT(returnPre()));                          //返回上一级
    connect(m_pDelFilePB, SIGNAL(clicked(bool)), this, SLOT(delRegFile()));                        //删除常规文件
    connect(m_pUploadPB, SIGNAL(clicked(bool)), this, SLOT(uploadFile()));                         //上传文件按钮
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(uploadFileData()));                            //上传文件数据
    connect(m_pDownLoadPB, SIGNAL(clicked(bool)), this, SLOT(downloadFile()));                     //下载文件按钮
    connect(m_pShareFilePB, SIGNAL(clicked(bool)), this, SLOT(shareFile()));                       //分享文件
    connect(m_pMoveFilePB, SIGNAL(clicked(bool)), this, SLOT(moveFile()));                         //移动文件按钮
    connect(m_pSelectDirPB, SIGNAL(clicked(bool)), this, SLOT(selectDestDir()));  //选择移动目的路径按钮
}

void Book::updateFileList(const PDU* pdu)  //更新文件夹列表
{
    if (NULL == pdu)  //判空
    {
        return;
    }
    m_pBookListW->clear();  //清空已有

    FileInfo* pFileInfo = NULL;
    int iCount = pdu->uiMsgLen / sizeof(FileInfo);  //有多少文件
    for (int i = 0; i < iCount; i++)
    {
        pFileInfo = (FileInfo*)(pdu->caMsg) + i;

        // qDebug() << pFileInfo->caFileName << pFileInfo->iFileType;

        QListWidgetItem* pItem = new QListWidgetItem;
        if (0 == pFileInfo->iFileType)
        {
            pItem->setIcon(QIcon(QPixmap(":/map/dir.png")));  //文件夹图标
        }
        else if (1 == pFileInfo->iFileType)
        {
            pItem->setIcon(QIcon(QPixmap(":/map/reg.png")));  //文件图标
        }

        pItem->setText(pFileInfo->caFileName);  //添加文件信息
        m_pBookListW->addItem(pItem);
    }
}

void Book::clearEnterDir()  //清空进入的文件路径
{
    m_strEnterDir.clear();
}

QString Book::enterDir()  //获取进入的文件路径
{
    return m_strEnterDir;
}

void Book::setDownloadStatus(bool status)  //设置下载状态
{
    m_bDownload = status;
}

bool Book::getDownloadStatus()  //获取下载状态
{
    return m_bDownload;
}
QString Book::getSaveFilePath()  //获取保存文件路径
{
    return m_strSaveFilePath;
}

QString Book::getShareFileName()  //获取分享文件名
{
    return m_strShareFileName;
}

void Book::createDir()  //创建文件夹按钮
{
    QString strNewDir = QInputDialog::getText(this, "新建文件夹", "新文件夹名字");  //新文件夹名字
    if (!strNewDir.isEmpty())
    {
        if (strNewDir.size() > 32)
        {
            QMessageBox::warning(this, "新建文件夹", "新文件夹名字不能超过32个字符");  //新文件夹名字
        }
        else
        {
            QString strName = TcpClient::getInstance().loginName();   //当前用户名
            QString strCurPath = TcpClient::getInstance().curPath();  //当前目录

            PDU* pdu = mkPDU(strCurPath.toUtf8().size() + 1);  // pdu
            pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
            strncpy(pdu->caData, strName.toStdString().c_str(), strName.toUtf8().size());           //用户名
            strncpy(pdu->caData + 32, strNewDir.toStdString().c_str(), strNewDir.toUtf8().size());  //新文件夹名
            memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.toUtf8().size());       //当前目录

            TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);  //发送
            free(pdu);
            pdu = NULL;
        }
    }
    else
    {
        QMessageBox::warning(this, "新建文件夹", "新文件夹名字不能为空");
    }
}

void Book::flushFile()  //刷新文件列表按钮
{
    QString strCurPath = TcpClient::getInstance().curPath();  //当前路径

    PDU* pdu = mkPDU(strCurPath.toUtf8().size() + 1);  // pdu
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    strncpy((char*)(pdu->caMsg), strCurPath.toStdString().c_str(), strCurPath.toUtf8().size());

    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);  //发送
    free(pdu);
    pdu = NULL;
}

void Book::delDir()  //删除文件夹按钮
{
    QString strCurPath = TcpClient::getInstance().curPath();  //当前路径

    QListWidgetItem* pItem = m_pBookListW->currentItem();  //当前选中

    if (NULL == pItem)  //判空
    {
        QMessageBox::warning(this, "删除文件", "请选择要删除的文件");
    }
    else
    {
        QString strDelName = pItem->text();  //文件夹名
        PDU* pdu = mkPDU(strCurPath.toUtf8().size() + 1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_REQUEST;

        strncpy(pdu->caData, strDelName.toStdString().c_str(), strDelName.toUtf8().size());  //文件夹名
        memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.toUtf8().size());    //当前目录

        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);  //发送
        free(pdu);
        pdu = NULL;
    }
}

void Book::renameFile()  //重命名文件按钮
{
    QString strCurPath = TcpClient::getInstance().curPath();  //当前路径
    QListWidgetItem* pItem = m_pBookListW->currentItem();     //当前选中

    if (NULL == pItem)  //判空
    {
        QMessageBox::warning(this, "重命名文件", "请选择要重命名的文件");
    }
    else
    {
        QString strOldName = pItem->text();                                                  //旧名字
        QString strNewName = QInputDialog::getText(this, "重命名文件", "请输入新的文件名");  //输入新名字

        if (!strNewName.isEmpty())  //判空
        {
            PDU* pdu = mkPDU(strCurPath.toUtf8().size() + 1);  // pdu
            pdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_REQUEST;

            strncpy(pdu->caData, strOldName.toStdString().c_str(), strOldName.toUtf8().size());       //旧名字
            strncpy(pdu->caData + 32, strNewName.toStdString().c_str(), strNewName.toUtf8().size());  //新名字
            memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.toUtf8().size());         //当前路径

            TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);  //发送
            free(pdu);
            pdu = NULL;
        }
        else
        {
            QMessageBox::warning(this, "重命名文件", "新文件名不能为空");
        }
    }
}

void Book::enterDir(const QModelIndex& index)  //进入文件夹按钮
{
    QString strDirName = index.data().toString();             //选中项名字
    m_strEnterDir = strDirName;                               //保存进入的目录
    QString strCurPath = TcpClient::getInstance().curPath();  //当前路径

    PDU* pdu = mkPDU(strCurPath.toUtf8().size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_REQUEST;
    strncpy(pdu->caData, strDirName.toStdString().c_str(), strDirName.toUtf8().size());  //要进入的目录名
    memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.toUtf8().size());    //当前目录

    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);  //发送
    free(pdu);
    pdu = NULL;
}

void Book::returnPre()  //返回上一级按钮
{
    QString strCurPath = TcpClient::getInstance().curPath();            //当前目录
    QString strRootPath = "./" + TcpClient::getInstance().loginName();  //根目录

    // qDebug() << strCurPath << strRootPath;
    if (strCurPath == strRootPath)  //在根目录
    {
        QMessageBox::warning(this, "返回", "返回失败：已经在最开始的文件夹目录中");
    }
    else
    {
        int index = strCurPath.lastIndexOf('/');              //最后一个‘/’的索引
        strCurPath.remove(index, strCurPath.size() - index);  //去掉最后一级
        // qDebug() << "return --> " << strCurPath;
        TcpClient::getInstance().setCurPath(strCurPath);  //设置当前目录

        clearEnterDir();  //清空已进入目录

        flushFile();  //刷新文件列表
    }
}

void Book::delRegFile()  //删除常规文件按钮
{
    QString strCurPath = TcpClient::getInstance().curPath();  //当前路径
    QListWidgetItem* pItem = m_pBookListW->currentItem();     //选中文件
    if (NULL == pItem)
    {
        QMessageBox::warning(this, "删除文件", "请选择要删除的文件");
    }
    else
    {
        QString strDelName = pItem->text();  //文件名

        PDU* pdu = mkPDU(strCurPath.toUtf8().size() + 1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_REQUEST;
        strncpy(pdu->caData, strDelName.toStdString().c_str(), strDelName.toUtf8().size());  //删除文件名
        memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.toUtf8().size());    //当前路径
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);            //发送
        free(pdu);
        pdu = NULL;
    }
}

void Book::uploadFile()  //上传文件按钮
{
    m_strUploadFilePath = QFileDialog::getOpenFileName();  //打开对话框，获取要上传文件的路径
    // qDebug() << m_strUploadFilePath;
    if (!m_strUploadFilePath.isEmpty())  //判空
    {
        int index = m_strUploadFilePath.lastIndexOf('/');  //最后一个‘/’的索引
        QString strFileName = m_strUploadFilePath.right(m_strUploadFilePath.size() - index - 1);  //获取文件名
        // qDebug() << strFileName;

        QFile file(m_strUploadFilePath);  //创建文件对象
        qint64 fileSize = file.size();    //获得文件大小

        QString strCurPath = TcpClient::getInstance().curPath();  //当前路径

        PDU* pdu = mkPDU(strCurPath.toUtf8().size() + 1);  // pdu
        pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
        memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.toUtf8().size());  //当前路径
        sprintf(pdu->caData, "%s %lld", strFileName.toStdString().c_str(), fileSize);      //文件名和文件大小

        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);  //发送
        free(pdu);
        pdu = NULL;

        m_pTimer->start(1000);  //启动计时器等待1s，防止粘包
    }
    else
    {
        QMessageBox::warning(this, "上传文件", "上传文件名字不能为空");
    }
}

void Book::uploadFileData()  //计时器超时后上传数据
{
    m_pTimer->stop();                     //关掉计时器
    QFile file(m_strUploadFilePath);      //要上传的文件
    if (!file.open(QIODevice::ReadOnly))  //只读打开
    {
        QMessageBox::warning(this, "上传文件", "打开文件失败");
        return;
    }

    char* pBuffer = new char[4096];  //发送缓冲区
    qint64 ret = 0;
    while (true)  //读取文件内容发送
    {
        ret = file.read(pBuffer, 4096);  //每次读这么多
        if (ret > 0 && ret <= 4096)      //读到数据大小正常
        {
            TcpClient::getInstance().getTcpSocket().write(pBuffer, ret);  //发送
        }
        else if (0 == ret)  //没东西了
        {
            break;
        }
        else  //错误
        {
            QMessageBox::warning(this, "上传文件", "上传文件失败:读文件失败");
            break;
        }
    }
    file.close();  //关闭文件
    delete[] pBuffer;
    pBuffer = NULL;
}

void Book::downloadFile()  //下载文件按钮
{
    QListWidgetItem* pItem = m_pBookListW->currentItem();  //当前选择的文件
    if (NULL == pItem)                                     //判空
    {
        QMessageBox::warning(this, "下载文件", "请选择要下载的文件");
    }
    else
    {
        QString strSaveFilePath = QFileDialog::getSaveFileName();  //选择保存路径窗口
        if (strSaveFilePath.isEmpty())                             //判空
        {
            QMessageBox::warning(this, "下载文件", "请指定要保存的位置");
            m_strSaveFilePath.clear();  //清空之前的保存路径
        }
        else
        {
            m_strSaveFilePath = strSaveFilePath;  //更新保存路径
        }

        QString strCurPath = TcpClient::getInstance().curPath();  //当前路径

        PDU* pdu = mkPDU(strCurPath.toUtf8().size() + 1);  // pdu
        pdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
        QString strFileName = pItem->text();                                               //选中文件名
        strcpy(pdu->caData, strFileName.toStdString().c_str());                            //文件名
        memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.toUtf8().size());  //当前路径

        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);  //发送
    }
}

void Book::shareFile()  //分享文件按钮
{
    QListWidgetItem* pItem = m_pBookListW->currentItem();  //当前选中文件
    if (NULL == pItem)                                     //判空
    {
        QMessageBox::warning(this, "分享文件", "请选择要分享的文件");
        return;
    }
    else
    {
        m_strShareFileName = pItem->text();  //分享文件名
    }

    Friend* pFriend = OpeWidget::getInstance().getFriend();  //获取当前好友窗口
    QListWidget* pFriendList = pFriend->getFriendList();     //获取好友列表

    // qDebug() << "bookupdate" << pFriendList->count() << endl;

    ShareFile::getInstance().updateFriend(pFriendList);  //更新分享窗口的好友列表
    if (ShareFile::getInstance().isHidden())             //打开分享窗口
    {
        ShareFile::getInstance().show();
    }
}

void Book::moveFile()  //移动文件按钮
{
    QListWidgetItem* pCurItem = m_pBookListW->currentItem();  //当前选中文件
    if (NULL != pCurItem)                                     //判空
    {
        m_strMoveFileName = pCurItem->text();                      //要移动文件名
        QString strCutPath = TcpClient::getInstance().curPath();   //当前路径
        m_strMoveFilePath = strCutPath + '/' + m_strMoveFileName;  //拼接路径

        m_pSelectDirPB->setEnabled(true);  //设置可选
    }
    else
    {
        QMessageBox::warning(this, "移动文件", "请选择要移动的文件");
    }
}
void Book::selectDestDir()
{
    QListWidgetItem* pCurItem = m_pBookListW->currentItem();  //当前所选
    if (NULL != pCurItem)
    {
        QString strDestDir = pCurItem->text();                    //所选文件
        QString strCutPath = TcpClient::getInstance().curPath();  //当前路径
        m_strDestDir = strCutPath + '/' + strDestDir;             //拼接

        int srcLen = m_strMoveFilePath.toUtf8().size();  //原路径长
        int destLen = m_strDestDir.toUtf8().size();      //目标长

        PDU* pdu = mkPDU(srcLen + destLen + 2);
        pdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_REQUEST;
        sprintf(pdu->caData, "%d %d %s", srcLen, destLen, m_strMoveFileName.toStdString().c_str());

        memcpy(pdu->caMsg, m_strMoveFilePath.toStdString().c_str(), srcLen);                      //原路径
        memcpy((char*)(pdu->caMsg) + (srcLen + 1), m_strDestDir.toStdString().c_str(), destLen);  //目标路径

        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);  //发送请求
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::warning(this, "移动文件", "请选择要移动的文件");
    }
    m_pSelectDirPB->setEnabled(false);  //设置不不可选
}

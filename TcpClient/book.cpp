#include "book.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "opewidget.h"
#include "tcpclient.h"

Book::Book(QWidget* parent) : QWidget(parent)
{
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
    //    m_pMoveFilePB = new QPushButton("移动文件");
    //    m_pSelectDirPB = new QPushButton("目标目录");
    // m_pSelectDirPB->setEnabled(false);

    QVBoxLayout* pFileVBL = new QVBoxLayout;  //垂直布局
    pFileVBL->addWidget(m_pUploadPB);
    pFileVBL->addWidget(m_pDownLoadPB);
    pFileVBL->addWidget(m_pDelFilePB);
    pFileVBL->addWidget(m_pShareFilePB);
    //    pFileVBL->addWidget(m_pMoveFilePB);
    //    pFileVBL->addWidget(m_pSelectDirPB);

    QHBoxLayout* pMain = new QHBoxLayout;  //水平布局
    pMain->addWidget(m_pBookListW);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);

    setLayout(pMain);

    connect(m_pCreateDirPB, SIGNAL(clicked(bool)), this, SLOT(createDir()));  //创建文件夹
    connect(m_pFlushFilePB, SIGNAL(clicked(bool)), this, SLOT(flushFile()));  //刷新文件列表
    //    connect(m_pDelDirPB, SIGNAL(clicked(bool)), this, SLOT(delDir()));

    //    connect(m_pRenamePB, SIGNAL(clicked(bool)), this, SLOT(renameFile()));

    //    connect(m_pBookListW, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(enterDir(QModelIndex)));

    //    connect(m_pReturnPB, SIGNAL(clicked(bool)), this, SLOT(returnPre()));

    //    connect(m_pDelFilePB, SIGNAL(clicked(bool)), this, SLOT(delRegFile()));

    //    connect(m_pUploadPB, SIGNAL(clicked(bool)), this, SLOT(uploadFile()));

    //    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(uploadFileData()));

    //    connect(m_pDownLoadPB, SIGNAL(clicked(bool)), this, SLOT(downloadFile()));

    //    connect(m_pShareFilePB, SIGNAL(clicked(bool)), this, SLOT(shareFile()));
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

// void Book::clearEnterDir() { m_strEnterDir.clear(); }

// QString Book::enterDir() { return m_strEnterDir; }

// void Book::setDownloadStatus(bool status) { m_bDownload = status; }

void Book::createDir()  //创建文件夹
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

#include "book.h"

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
}

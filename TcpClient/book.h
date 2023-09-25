#ifndef BOOK_H
#define BOOK_H

#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "protocol.h"

/*
    图书
*/

class Book : public QWidget
{
    Q_OBJECT
  public:
    explicit Book(QWidget* parent = nullptr);
    void updateFileList(const PDU* pdu);  //更新文件列表
                                          //    void clearEnterDir();
                                          //    QString enterDir();
                                          //    void setDownloadStatus(bool status);
                                          //    bool getDownloadStatus();
                                          //    QString getSaveFilePath();
                                          //    QString getShareFileName();

    //    qint64 m_iTotal;    //总的文件大小
    //    qint64 m_iRecved;   //已收到多少

  signals:

  public slots:
    void createDir();  //创建文件夹
    void flushFile();  //刷新文件列表
                       //    void delDir();
                       //    void renameFile();
                       //    void enterDir(const QModelIndex& index);
                       //    void returnPre();
                       //    void delRegFile();
                       //    void uploadFile();

    //    void uploadFileData();

    //    void downloadFile();

    //    void shareFile();
    //    void moveFile();
    //    void selectDestDir();

  private:
    QListWidget* m_pBookListW;    //文件名列表
    QPushButton* m_pReturnPB;     //返回按钮
    QPushButton* m_pCreateDirPB;  //创建文件夹按钮
    QPushButton* m_pDelDirPB;     //删除文件夹按钮
    QPushButton* m_pRenamePB;     //重命名按钮
    QPushButton* m_pFlushFilePB;  //刷新文件按钮
    QPushButton* m_pUploadPB;     //上传文件按钮
    QPushButton* m_pDownLoadPB;   //下载文件按钮
    QPushButton* m_pDelFilePB;    //删除文件按钮
    QPushButton* m_pShareFilePB;  //分享文件按钮
    //    QPushButton* m_pMoveFilePB;   //移动文件按钮
    //    QPushButton* m_pSelectDirPB;  //目标目录按钮

    QString m_strEnterDir;
    //    QString m_strUploadFilePath;

    //    QTimer* m_pTimer;

    //    QString m_strSaveFilePath;
    //    bool m_bDownload;

    //    QString m_strShareFileName;

    //    QString m_strMoveFileName;
    //    QString m_strMoveFilePath;
    //    QString m_strDestDir;
};

#endif  // BOOK_H

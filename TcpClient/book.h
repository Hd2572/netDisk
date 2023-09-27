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
    void clearEnterDir();                 //清空进入的文件路径
    QString enterDir();                   //获取进入的文件路径
                                          //    void setDownloadStatus(bool status);
                                          //    bool getDownloadStatus();
                                          //    QString getSaveFilePath();
                                          //    QString getShareFileName();

    //    qint64 m_iTotal;    //总的文件大小
    //    qint64 m_iRecved;   //已收到多少

  signals:

  public slots:
    void createDir();                         //创建文件夹按钮
    void flushFile();                         //刷新文件列表
    void delDir();                            //删除文件夹按钮
    void renameFile();                        //重命名文件按钮
    void enterDir(const QModelIndex& index);  //双击进入文件夹按钮
    void returnPre();                         //返回上一级按钮
    void delRegFile();                        //删除常规文件按钮
    void uploadFile();                        //上传文件按钮

    void uploadFileData();  //上传文件数据

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

    QString m_strEnterDir;        //进入的目录
    QString m_strUploadFilePath;  //上传文件的路径

    QTimer* m_pTimer;  //传文件定时器

    //    QString m_strSaveFilePath;
    //    bool m_bDownload;

    //    QString m_strShareFileName;

    //    QString m_strMoveFileName;
    //    QString m_strMoveFilePath;
    //    QString m_strDestDir;
};

#endif  // BOOK_H

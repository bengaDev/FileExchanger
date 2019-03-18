#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#define SERVER_PORT 2015
#define BROADCAST_PORT 2016
#define BOOT_TOOL_PORT 2017
#define PAYLOAD_SIZE 1024
#define FILE_PARTITION_SIZE 500*1024*1024
//#define SERVER_PORT3 2017

#include <list>
#include <host.h>
#include <QMutex>
#include <QFile>




class Data_Manager : public QObject
{
    Q_OBJECT

public:
    Data_Manager(QString localHostName, QString fileName);
    Host *localHost;
    void deleteOnlineUser(QUuid hostID);
    void deleteAllToSendUsers();
    bool isPresentInOnlineUsers(QUuid);
    bool isPresentInToSendUsers(QUuid);
    void addQueueNextOnlineUsers(Host);
    void setAvatarOfNextOnlineUser(QPixmap, QUuid);
    void DEBUG_clearOnlineUsers();
    std::list<Host> getOnlineUsers();
    std::list<Host> getToSendUsers();
    QString getFilePath();
    QString getFileName();
    void refreshOnlineUsers();
    uint getRefreshTime();
    void updateHostInfo(QUuid uniqueID, time_t time, QString name);
    void setLocalHostVisibility(bool);
    void setLocalHostName(QString);
    void setReceiveFilesAutom(bool);
    bool getReceiveFilesAutom();
    void setIFDefaultSavingPath(bool);
    bool getIFDefaultSavingPath();
    void setFilePath(QString path, bool isDir);
    bool getIsDir();

signals:
    void isUpdated();
    void activateMainWindow(QString fileName);
    void sendFile_SIGNAL();
    void setProgBarMaximum_SENDER(QUuid, qint64);
    void setProgBarValue_SENDER(QUuid, qint64);
    void setProgBarMaximum_RECEIVER(QUuid, qint64);
    void setProgBarValue_RECEIVER(QUuid, qint64);
    void setLabelProgBarWindow(QUuid, QString);
    void closeSocket();

    // Signals for GUI-ReceiverWorker communication
    void metadataStageEND(qint64, QString, QUuid, QString);
    void messageBoxYes();
    void savingPath(QString);

    // Signal for Closing application
    void quittingApplication();

    //Signal for transmission interruption by user pushbutton
    void interruptSending(QUuid);
    void interruptReceiving(QUuid);
    void interruptAllSending();
    void interruptAllReceiving();

    void fileOpenError();

    void sendNewFile(QString);
    void endSendingFile(QString);

public slots:
    void addToSendUser(QUuid uniqueID);
    void deleteToSendUser(QUuid uniqueID);
    void DEBUG_trySlot(QUuid);

private:
    QString filePath = nullptr;
    QString fileName = nullptr;
    bool isDir;
    std::list<Host> onlineUsers;
    std::list<Host> toSend;
    std::list<Host> queueNextOnlineUsers;
    uint refreshTime = 10;
    bool receiveFileAutomatically = false;
    bool defaultSavingPath = true;

    QMutex mutex;
    //std::string file_path;

    void addOnlineUser(Host newHost);
};


#endif // DATA_MANAGER_H

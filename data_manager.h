#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#define SERVER_PORT 2015
#define BROADCAST_PORT 2016
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
    void addOnlineUser(Host newHost);
    void deleteOnlineUser(Host newHost);
    bool isPresentInOnlineUsers(QUuid);
    void addQueueNextOnlineUsers(Host);
    void setAvatarOfNextOnlineUser(QPixmap, QUuid);
    void DEBUG_clearOnlineUsers();
    std::list<Host> getOnlineUsers();
    std::list<Host> getToSendUsers();
    QFile* getFileToSend();
    QString getFileName();

signals:
    void isUpdated();
    void sendFile_SIGNAL();
    void setProgBarMaximum_SIGNAL(QUuid, qint64);
    void setProgBarValue_SIGNAL(QUuid, qint64);

public slots:
    void addToSendUsers(QUuid uniqueID);
    void deleteToSendUsers(QUuid uniqueID);
    void DEBUG_trySlot(QUuid, qint64);

private:
    QFile *fileToSend = nullptr;
    QString fileName = nullptr;
    std::list<Host> onlineUsers; // le liste devono avere un lock?
    std::list<Host> toSend;
    std::list<Host> queueNextOnlineUsers;

    QMutex mutex;
    //std::string file_path;
};


#endif // DATA_MANAGER_H

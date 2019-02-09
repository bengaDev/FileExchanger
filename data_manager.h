#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H
#include <list>
#include <host.h>
#include <QMutex>

#define BROADCAST_PORT 2016
#define SERVER_PORT 2015

class Data_Manager : public QObject
{
    Q_OBJECT

public:
    Data_Manager(QString localHostName);
    Host *localHost;
    void addOnlineUser(Host newHost);
    void deleteOnlineUser(Host newHost);
    bool isPresentInOnlineUsers(QUuid);
    void addQueueNextOnlineUsers(Host);
    void setAvatarOfNextOnlineUser(QPixmap, QUuid);
    void DEBUG_clearOnlineUsers();
    std::list<Host> getOnlineUsers();

signals:
    void isUpdated();

public slots:
    void addToSendUsers(QUuid uniqueID);
    void deleteToSendUsers(QUuid uniqueID);

private:
    std::list<Host> onlineUsers; // le liste devono avere un lock?
    std::list<Host> toSend;
    std::list<Host> queueNextOnlineUsers;

    QMutex mutex;
    //std::string file_path;
};


#endif // DATA_MANAGER_H

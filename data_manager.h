#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H
#include <list>
#include <host.h>

class Data_Manager : public QObject
{
    Q_OBJECT

public:
    Data_Manager(QString localHostName);
    Host *localHost;
    void addOnlineUser(Host newHost);
    void deleteOnlineUser(Host newHost);
    bool isPresentInOnlineUsers(QUuid);
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
    //std::string file_path;
};


#endif // DATA_MANAGER_H

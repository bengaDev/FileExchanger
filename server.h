#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <data_manager.h>

class Server
{
public:
    Server();
private:
    QTcpServer tcpServer;
    Data_Manager *dm = nullptr;
};

#endif // SERVER_H

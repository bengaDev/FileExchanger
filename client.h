#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <data_manager.h>

class Client
{
public:
    Client();
private:
    Data_Manager *dm;
    int refreshTime = 30;

};

#endif // CLIENT_H

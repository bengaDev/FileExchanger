#ifndef FORTUNESERVER_H
#define FORTUNESERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include "data_manager.h"


class FortuneServer : public QTcpServer
{
    Q_OBJECT

private:
    Data_Manager *dm;
public:
    FortuneServer(Data_Manager* dm, QObject *parent = 0);
    static void threadFunction(Data_Manager*, qintptr);

protected:
    void incomingConnection(qintptr) override;
};

#endif // FORTUNESERVER_H

#ifndef FORTUNESERVER_H
#define FORTUNESERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include <QThread>
#include "data_manager.h"
#include "receiverworker.h"


class FortuneServer : public QTcpServer
{
    Q_OBJECT

private:
    Data_Manager *dm;
public:
    FortuneServer(Data_Manager* dm, QObject *parent = 0);
    void threadFunction(Data_Manager*, qintptr);

protected:
    void incomingConnection(qintptr) override;
};

#endif // FORTUNESERVER_H

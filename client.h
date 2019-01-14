#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QTcpServer>
#include <QThread>
#include <data_manager.h>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(Data_Manager *dm, QObject *parent = nullptr);

public slots:
    void hello();

private:
    Data_Manager *dm = nullptr;
    uint refreshTime = 10;
    QTcpSocket *tcpSocket = nullptr;

};

#endif // CLIENT_H

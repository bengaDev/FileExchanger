#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>
#include <QtConcurrent/QtConcurrent>
#include <QThread>
#include <data_manager.h>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(Data_Manager *dm, QObject *parent = nullptr);

public slots:


private:
    Data_Manager *dm = nullptr;
    uint refreshTime = 10;
    QTcpSocket *tcpSocket = nullptr;
    QThread *broadcastThread;

    static void hello(Data_Manager*);
    //void (Client::*p)() = &Client::hello;
    QTcpSocket *broadcastTcpSocket;

};

#endif // CLIENT_H

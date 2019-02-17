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
    ~Client();

public slots:
    void on_UdpReceive();
    void sendFile();


private:
    Data_Manager *dm = nullptr;
    uint refreshTime = 10;
    QTcpSocket *tcpSocket;
    QUdpSocket *udpSocket;
    QThread *broadcastThread;

    void hello();
    void sendAvatar(QHostAddress);
    void sendFileToUser(Host h);
    //void (Client::*p)() = &Client::hello;
    QTcpSocket *broadcastTcpSocket;

    QAtomicInt atomicLoopFlag = 1;

};

#endif // CLIENT_H

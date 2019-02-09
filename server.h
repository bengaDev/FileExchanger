#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QtConcurrent/QtConcurrent>
#include <QString>
#include "data_manager.h"
#include "fortuneserver.h"

/*
 * MESSAGE FROM EXTERNAL HOST TO MY SERVER:
 *
 * VISIBILITY MESSAGE: <HOST_QUid>_<HostName>_<visibility(bool)> ----------> Rember to check ip correctness!!
 * REQUEST AVATAR MESSAGE: <HOST_QUid>_sendAvatar_
 * AVATAR INCOMING: <HOST_QUid>_<avatar_in_byte_array>
 *  *QUid not necessary if using the same connection*
 *
* The server will receive <QUid>_<visibility(bool)>
* Avatar can be requested, if Quid not present:
*
*
*/

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(Data_Manager *dm, QObject *parent = nullptr);

signals:
    //void newConnecion(); already present in QTcpServer
public slots:
    void newConnectionSLOT(); //to manage signal newConnection from QTcpServer
    void readBroadcastDatagram();
    //void readTcpData();

private:
    QTcpServer *tcpServer;
    QUdpSocket *udpSocket;
    QTcpSocket *tcpSocket, *tcpSocketAvatar;
    FortuneServer *fortuneServer;
    quint16 port = SERVER_PORT; //use this port or example
    Data_Manager *dm = nullptr;
    int count = 0;

    static void readTcpData(qintptr);

protected:
    //void incomingConnection(qintptr socketDescriptor) override;
};

#endif // SERVER_H

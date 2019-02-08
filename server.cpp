#include "server.h"

Server::Server(Data_Manager *dm, QObject *parent) :
    QObject (parent)
{
    this->dm = dm;

    // ======== BROADCAST UDP RECEIVE ========
    udpSocket = new QUdpSocket(this);
    // 'ShareAddress' allows other services to bind to the same address and port
    // In order to have exclusive binding of port and address in this "service", use: 'DontShareAddress'
    if(!udpSocket->bind(1515, QUdpSocket::ShareAddress)){
        // UDP listener of server could not start
        qDebug() << "Server: UDP listener could not start!";
    } else {
        // UDP listener started
        qDebug() << "Server: UDP listener started!";
    }
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readBroadcastDatagram()));


    // ======== TCP CONNECTIONS ========
    tcpServer = new QTcpServer(this);


    ///connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnectionSLOT()));
    ///connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readTcpData()));

    fortuneServer = new FortuneServer(dm);
    if(!fortuneServer->listen(QHostAddress::Any, 1515) ){
         qDebug() << "FortuneServer: could not start!";
    }
}

void Server::readBroadcastDatagram(){
    QByteArray datagram;
    QHostAddress senderIP;
    QString datagramString;
    QStringList datagramTokens;

    QString uniqueID, name, visibility;

    while (udpSocket->hasPendingDatagrams()) {
        datagram.resize(int(udpSocket->pendingDatagramSize()));
        udpSocket->readDatagram(datagram.data(), datagram.size(), &senderIP);

        datagramString = QString(datagram);

        if(datagram.startsWith("HOST Basic Info:")){
            qDebug() << "Server: received datagram with basic info -- UDP";

            datagramString.remove("HOST Basic Info:");
            datagramTokens = datagramString.split("_");

            uniqueID = datagramTokens.at(0);
            name = datagramTokens.at(1);
            visibility = datagramTokens.at(2);

            uniqueID.remove('{');
            uniqueID.remove('}');
            qDebug() << "Sender IP: " << senderIP.toString() << " || My IP: " << QHostAddress(QHostAddress::LocalHost);

            // Now basic info is acquired. I need to check if the received datagram is from a visible
            // host and if it is not already present in the 'onlineUsers' list.
            // If these two requirements are satisfied -> send me your avatar message
            if(visibility == "VISIBLE" && dm->isPresentInOnlineUsers(QUuid(uniqueID)) == false){
                // I want the rest of the data - Server responds in UDP
                qDebug() << "Server: request for more info -- UDP";
                Host h(true, name);
                h.setUniqueID(uniqueID);
                dm->addQueueNextOnlineUsers(h);  //queue without avatar

                QByteArray moreInfoDatagram = "SERVER REQUEST more info";
                //QUdpSocket udpSocket_2;
                udpSocket->writeDatagram(moreInfoDatagram, senderIP, 1516);
                udpSocket->waitForBytesWritten(5000);
            }  //add else not visible....

        } else {
            qDebug() << "Server: " << "Not a valid message!";
        }
    }
}

void Server::newConnectionSLOT() //action performed each timen a new connection arrives
{
    //QTcpSocket *socket = tcpServer->nextPendingConnection();
    QByteArray sentData;

    qintptr socketDescriptor = tcpServer->nextPendingConnection()->socketDescriptor();

    QtConcurrent::run(this->readTcpData, socketDescriptor);


}

void Server::readTcpData(qintptr socketDescriptor){

    QByteArray sentData;

    QTcpSocket tcpSocket;

    if (!tcpSocket.setSocketDescriptor(socketDescriptor)) {
        //emit error(tcpSocket.error());
        qDebug() << "Server: error in opening socket thorugh 'socketDescriptor' - Threaded";
        return;
    }


    tcpSocket.waitForReadyRead(5000);

    sentData = tcpSocket.readAll();

    if(sentData.startsWith("avatar")){
        qDebug() << "Avatar received";
        tcpSocket.disconnectFromHost();
    }

}


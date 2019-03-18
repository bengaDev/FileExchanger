#include "server.h"

Server::Server(Data_Manager *dm, QObject *parent) :
    QObject (parent)
{
    this->dm = dm;

    // ======== BROADCAST UDP RECEIVE ========
    udpSocket = new QUdpSocket(this);
    // 'ShareAddress' allows other services to bind to the same address and port
    // In order to have exclusive binding of port and address in this "service", use: 'DontShareAddress'
    if(!udpSocket->bind(SERVER_PORT, QUdpSocket::ShareAddress)){
        // UDP listener of server could not start
        qDebug() << "Server: UDP listener could not start!";
    } else {
        // UDP listener started
        qDebug() << "Server: UDP listener started!";
    }
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readBroadcastDatagram()));


    // ======== Boot Tool UDP RECEIVE ========
    udpSocketTool = new QUdpSocket(this);
    // 'ShareAddress' allows other services to bind to the same address and port
    // In order to have exclusive binding of port and address in this "service", use: 'DontShareAddress'
    if(!udpSocketTool->bind(QHostAddress::LocalHost, BOOT_TOOL_PORT, QUdpSocket::ShareAddress)){
        // UDP listener of server could not start
        qDebug() << "Server: UDP listener could not start!";
    } else {
        // UDP listener started
        qDebug() << "Server: UDP listener started!";
    }
    connect(udpSocketTool, SIGNAL(readyRead()), this, SLOT(readBootToolDatagram()));


    // ======== TCP CONNECTIONS ========
    //tcpServer = new QTcpServer(this);


    ///connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnectionSLOT()));
    ///connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readTcpData()));

    fortuneServer = new FortuneServer(dm);
    if(!fortuneServer->listen(QHostAddress::Any, SERVER_PORT) ){
         qDebug() << "FortuneServer: could not start!";
    }
}

void Server::readBroadcastDatagram(){
    QByteArray datagram;
    QHostAddress senderIP;
    QString datagramString;
    QStringList datagramTokens;
    time_t receivedTime = time(nullptr);

    QString uniqueID, name, visibility;

    qDebug() << "Server: readBroadcast datagram!";


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
            if(visibility == "VISIBLE" && dm->isPresentInOnlineUsers(QUuid(uniqueID)) == true){
                // refresh last seen time
               dm->updateHostInfo(QUuid(uniqueID), receivedTime, name);

            }else if(visibility == "NOTVISIBLE" && dm->isPresentInOnlineUsers(QUuid(uniqueID)) == true){
                dm->deleteOnlineUser(QUuid(uniqueID));
            }else if(visibility == "VISIBLE" && dm->isPresentInOnlineUsers(QUuid(uniqueID)) == false && QUuid(uniqueID) != dm->localHost->getUniqueID()){
                // I want the rest of the data - Server responds in UDP
                qDebug() << "Server: requesting for more info -- UDP";
                Host h(true, name);
                h.setUniqueID(uniqueID);
                h.setIP(senderIP);
                h.setLastSeen(receivedTime);
                dm->addQueueNextOnlineUsers(h);  //queue without avatar

                QByteArray moreInfoDatagram = "SERVER REQUEST more info";
                //QUdpSocket udpSocket_2;
                udpSocket->writeDatagram(moreInfoDatagram, senderIP, BROADCAST_PORT);
                udpSocket->waitForBytesWritten(5000);
            }

        } else {
            qDebug() << "Server: " << "Not a valid message!";
        }
    }
}

void Server::readBootToolDatagram(){ // main thread
    QByteArray datagram;
    QString datagramString;
    QStringList datagramTokens;

    qDebug() << "Server: received BootToolDatagram!";

    while (udpSocketTool->hasPendingDatagrams()) {
        datagram.resize(int(udpSocketTool->pendingDatagramSize()));
        udpSocketTool->readDatagram(datagram.data(), datagram.size());

        datagramString = QString(datagram);

        if(datagram.startsWith("BootTool isDir")){
            qDebug() << "Server: received datagram with basic info -- UDP";
            datagramTokens = datagramString.split("?");
            if(datagramTokens[1] == "YES"){
                qDebug() << "RECEIVED A DIR PATH " + datagramTokens[2];

                dm->setFilePath(datagramTokens[2], true);

            }else if(datagramTokens[1] == "NO"){
                qDebug() << "RECEIVED A FILE PATH " + datagramTokens[2];

                dm->setFilePath(datagramTokens[2], false);
            }

            emit dm->activateMainWindow(dm->getFileName());

        }
    }
}


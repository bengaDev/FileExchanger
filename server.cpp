#include "server.h"

Server::Server(QObject *parent) :
    QObject (parent)
{
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
    tcpSocket = new QTcpSocket(); // ADDED
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnectionSLOT()));

    if(!tcpServer->listen(QHostAddress::Any, port)) //listen to any ip address and on port = port
    {
        //SERVER COULD NOT START
        qDebug() << "Server: could not start!";
    }
    else
    {
        //SERVER STARTED
        qDebug() << "Server: started!";
        //TODO: ADD THREAD THAT SENDS EACH 15sec a isOnline message
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

        qDebug() << "Server: received datagram";

        datagramString = QString(datagram);

        if(datagram.startsWith("HOST Basic Info:")){
            qDebug() << "Server: " << datagramString << " -> is a valid address";
            qDebug() << "Server: IP=" << senderIP.toString();

            datagramString.remove("HOST Basic Info:");
            datagramTokens = datagramString.split("_");
            uniqueID = datagramTokens.at(0);
            name = datagramTokens.at(1);
            visibility = datagramTokens.at(2);

            // Now basic info is acquired. I need to check if the received datagram is from a visible
            // host and if it is not already present in the 'onlineUsers' list.
            // If these two requirements are satisfied -> send me your avatar message
            if(visibility == "VISIBLE" && dm->isPresentInOnlineUsers(uniqueID) == false){
                // I want the rest of the data
                tcpSocket->connectToHost(senderIP, 1515);
            }

        } else {
            qDebug() << "Server: " << "Not a valid message!";
        }
    }
}

void Server::newConnectionSLOT() //action performed each timen a new connection arrives
{
    QTcpSocket *socket = tcpServer->nextPendingConnection();

    this->count++;

    char message[50];

    qDebug() << "Server: new Connection arrived! n:" << this->count;
    //analyze the content of the message

    qDebug() << "Server: waiting for bytes to be written";

    socket->waitForReadyRead(5000); //aspetto 5 secondi dopodichè vado avanti

    //qDebug() << "Server: " << socket->read(message, 50);

    QString message_string = socket->readLine();

    qDebug() << "Server: " << message_string << " tot length: " << message_string.length();

    if(message_string.startsWith("IP:")){
        message_string = message_string.remove(0, 3);
        qDebug() << "Server: " << message_string << " -> is a valid ip! ";

        socket->write(message+3, 12);
    }
    else{
        qDebug() << "Server: " << "Not a valid message!";
    }

    socket->flush();

    socket->write("ByeBye");
    socket->waitForBytesWritten(3000);

    qDebug() << "Server: " << "Closing Socket!";

    socket->close();
}

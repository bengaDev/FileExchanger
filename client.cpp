#include "client.h"

/*
 * CHECK IF SOMETHING GOES WRONG!
 *
 * maybe a slot that manages error() signal is needed
 * or a try catch
 *
 * check how many sockets will be created in case of ANY connection
 *
*/

Client::Client(Data_Manager *dm, QObject *parent) :
    QObject (parent)
{
    this->dm = dm;
    qDebug() << "Client: Hello i'm " << dm->localHost->getName() <<
                "and my visibility is: " << dm->localHost->isVisible();
    qDebug() << "Client: " << dm->localHost->getUniqueID();

    udpSocket = new QUdpSocket(this);
    tcpSocket = new QTcpSocket(this);

    if(!udpSocket->bind(1516, QUdpSocket::ShareAddress)){
        // UDP listener of server could not start
        qDebug() << "Client: UDP listener could not start!";
    } else {
        // UDP listener started
        qDebug() << "Client: UDP listener started!";
    }

    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(on_UdpReceive()));
    //void (*p) () = this->hello();
    //void (Client::*helloFuncPointer) ();
    //helloFuncPointer = &Client::hello;
    //QThread::create(this->hello, dm);
    //broadcastThread = QThread::create(this->hello);
    //QtConcurrent::run(this->hello, dm);
    QtConcurrent::run(this, &Client::hello);


}

void Client::hello(){
    qDebug() << "Client: inizialization...";
    uint refreshTime = 10;
    QUdpSocket udpSocket;

    while(atomicLoopFlag == 1){
        QByteArray datagram = "HOST Basic Info:" + dm->localHost->getUniqueID().toByteArray()
                + "_" + dm->localHost->getName().toUtf8() + "_" + dm->localHost->getVisibilityStatus().toUtf8();
        qDebug() << "=======================================================\n";
        qDebug() << "Client: Broadcasting basic info -- UDP";

        if(udpSocket.writeDatagram(datagram, QHostAddress::Broadcast, 1515) == -1){
            qDebug() << "Client: Could not send broadcast basic info -- UDP";
        } else {
            qDebug() << "Client: Broadcast of basic info done -- UDP" << udpSocket.state();

        }

        QThread::sleep(refreshTime);

    }
}

void Client::on_UdpReceive(){
    QByteArray datagram;
    QHostAddress senderIP;
    QString datagramString;

    while (udpSocket->hasPendingDatagrams()) {
        datagram.resize(int(udpSocket->pendingDatagramSize()));
        udpSocket->readDatagram(datagram.data(), datagram.size(), &senderIP);

        datagramString = QString(datagram);

        if(datagram.startsWith("SERVER REQUEST more info")){
            // I want to send avatar on separate thread
            //sendAvatar(dm, senderIP);
            qDebug() << "Client: received request for more info -- UDP";
            QtConcurrent::run(this, &Client::sendAvatar, senderIP);
        }
    }


}

void Client::sendAvatar(QHostAddress senderIP){

    //QImage avatar(dm->localHost->getAvatar().toImage());

    QPixmap avatar(dm->localHost->getAvatar());
    QTcpSocket tcpSocket;
    tcpSocket.connectToHost(senderIP, 1515);

    QByteArray buffer;

    QDataStream out(&buffer, QIODevice::ReadWrite);

    out << qint32(0); // This inserts in buffer an int (4 bytes), where later the size will be stored
    out << "avatar of " + dm->localHost->getUniqueID().toByteArray();
    out << avatar; // binary representation of the image

    out.device()->seek(0);
    out << buffer.size(); // Here size of int(0)+avatar is inserted in place of int(0)

    qDebug() << "Client: sending avatar (separate thread)-- TCP";
    qDebug() << "Avatar size: " << buffer.size();

    if(tcpSocket.write(buffer) < buffer.size()){
        qDebug("Transmission error in sending avatar");
    }
    tcpSocket.waitForBytesWritten(5000);

    /*QByteArray pictureBin;
    QBuffer buffer(&pictureBin);
    avatar.save(&buffer, "PNG");

    QTcpSocket tcpSocket;
    tcpSocket.connectToHost(senderIP, 1515);

    qDebug() << "Client: sending avatar (separate thread)-- TCP";
    qDebug() << "Avatar size: " << pictureBin.size();

    //tcpSocket.write("avatar");
    tcpSocket.write("avatar of " + dm->localHost->getUniqueID().toByteArray() + "_" + pictureBin);
    tcpSocket.waitForBytesWritten(5000);*/


}

Client::~Client(){
    atomicLoopFlag = 0;
}

// noreturn can be omitted, but is better for compiler optimization
/*void Client::hello(){
    qDebug() << "Client: inizialization...";

    while(true){

        QThread::sleep(refreshTime);

        tcpSocket = new QTcpSocket(this);
        tcpSocket->connectToHost(QHostAddress::LocalHost, 1515);

        qDebug() << "Client: Connected";

        tcpSocket->write(dm->localHost->getUniqueID().toByteArray()); // add visibility (bool)

        tcpSocket->waitForBytesWritten(5000);

        qDebug() << "Client: closing connection";
        tcpSocket->close();


    }
}*/

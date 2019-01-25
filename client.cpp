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

    //void (*p) () = this->hello();
    //void (Client::*helloFuncPointer) ();
    //helloFuncPointer = &Client::hello;
    //QThread::create(this->hello, dm);
    //broadcastThread = QThread::create(this->hello);
    QtConcurrent::run(this->hello, dm);

}

void Client::hello(Data_Manager* dm){
    qDebug() << "Client: inizialization...";
    uint refreshTime = 10;
    QUdpSocket udpSocket;

    while(true){
        QByteArray datagram = "HOST Basic Info:" + dm->localHost->getUniqueID().toByteArray()
                + "_" + dm->localHost->getName().toUtf8() + "_" + dm->localHost->getVisibilityStatus().toUtf8();

        qDebug() << "Client: Broadcasting basic info -- " << udpSocket.state();
        if(udpSocket.writeDatagram(datagram, QHostAddress::Broadcast, 1515) == -1){
            qDebug() << "Client: Could not send broadcast datagram";
        } else {
            qDebug() << "Client: Broadcast of basic info done -- " << udpSocket.state();

        }


        //tcpSocket.close();

        QThread::sleep(refreshTime);

    }
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

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
}

// noreturn can be omitted, but is better for compiler optimization
void Client::hello(){
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
}

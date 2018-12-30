#include "server.h"

Server::Server(QObject *parent) :
    QObject (parent)
{
    tcpServer = new QTcpServer(this);

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));

    if(!tcpServer->listen(QHostAddress::Any, port)) //listen to any ip address and on port = port
    {
        //SERVER COULD NOT START
        qDebug() << "Server could not start!";
    }
    else
    {
        //SERVER STARTED
        qDebug() << "Server started!";
        //TODO: ADD THREAD THAT SENDS EACH 15sec a isOnline message
    }
}

void Server::newConnection() //action performed each timen a new connection arrives
{
    QTcpSocket *socket = tcpServer->nextPendingConnection();
    qDebug() << "New Connection arrived!";
    //analyze the content of the message
    /* 2 types of message:
     *
     * 1.
     *  Hello message:
     *      <IP>__[HEY/ZZZ]__
     *
     * 2.
     *  File Transfer Request
     *      <IP>__FileTransfer__
     *
     *
     */
    qDebug() << "Waiting for bytes to be written";

    for(int i = 0; i<3; i++) {
        socket->waitForReadyRead(5000);

        qDebug() << socket->readBufferSize();

        qDebug() << socket->readAll();

        socket->flush();

    }

    socket->write("ByeBye");
    socket->waitForBytesWritten(3000);
    qDebug() << "Closing Socket!";

    socket->close();
}

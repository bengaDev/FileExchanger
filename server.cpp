#include "server.h"

Server::Server(QObject *parent) :
    QObject (parent)
{
    tcpServer = new QTcpServer(this);

    connect(tcpServer, SIGNAL(newConnectionSLOT()), this, SLOT(newConnectionSLOT()));

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

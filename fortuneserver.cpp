#include "fortuneserver.h"
#include <QtConcurrent/QtConcurrent>

FortuneServer::FortuneServer(Data_Manager *dm, QObject *parent)
        : QTcpServer(parent)
{
    this->dm = dm;
}


void FortuneServer::incomingConnection(qintptr socketDescriptor){
    qDebug() << "Fortune Server: incoming connection (could be avatar or file)";
    QtConcurrent::run(this->threadFunction, dm, socketDescriptor);
}

void FortuneServer::threadFunction(Data_Manager* dm, qintptr socketDescriptor){
    QByteArray sentData;

    QTcpSocket tcpSocket;

    if (!tcpSocket.setSocketDescriptor(socketDescriptor)) {
        //emit error(tcpSocket.error());
        qDebug() << "Server: error in opening socket thorugh 'socketDescriptor' - Threaded";
        return;
    }


    tcpSocket.waitForReadyRead(5000);

    // READING DOESN'T WORK PROPERLY YET
    // Probably the data that is sent (the avatar) is too big and a while 'waitForReadyRead' is needed.
    // Also a better solution would probably be to use signals and slots
    sentData = tcpSocket.readAll();

    if(sentData.startsWith("avatar of ")){
        qDebug() << "Fortune Server: Avatar received (separate thread)";
        tcpSocket.disconnectFromHost();

        QStringList stringTokens;
        QString uniqueID;
        QBuffer *buffer = new QBuffer();

        sentData.remove(0, 10); // for 'avatar of '
        sentData.remove(0, 1); // for open brace '{'
        sentData.remove(36, 1); // for closed brace '}'

        QString data(sentData);
        stringTokens = data.split('_');
        uniqueID = stringTokens.at(0);

        sentData.remove(0, 37); // for QUuid delete

        buffer->write(sentData);
        buffer->seek(0);

        QPixmap image;
        QImage image_2;
        image.loadFromData(buffer->buffer());
        image_2.loadFromData(buffer->buffer());


    }
}

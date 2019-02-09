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
    QPixmap avatar;

    QTcpSocket tcpSocket;

    if (!tcpSocket.setSocketDescriptor(socketDescriptor)) {
        //emit error(tcpSocket.error());
        qDebug() << "Server: error in opening socket thorugh 'socketDescriptor' - Threaded";
        return;
    }

    tcpSocket.waitForReadyRead(5000);
    /*sentData = tcpSocket.readAll();
    sentData.remove(0,8); // removes number

    if(sentData.startsWith("avatar of ")){

    }*/
    QDataStream in(&tcpSocket);
    QString dataType;
    qint32 msgSize = -1;
    qint64 temp = tcpSocket.bytesAvailable();

    if(temp && msgSize==-1){
        in >> msgSize;
    }

    while(tcpSocket.bytesAvailable() < msgSize - 4 ){
        qDebug() << "Bytes Available (to be read) : " << tcpSocket.bytesAvailable();
        if(!tcpSocket.waitForReadyRead(5000)){
            tcpSocket.disconnectFromHost();
            break;
        }
    }

    in >> sentData;


    if(sentData.startsWith("avatar of ")){
        qDebug() << "Fortune Server: Avatar received (separate thread)";
        qDebug() << "Received data size: " << sentData.size();
        //tcpSocket.disconnectFromHost();
        in >> avatar;

        qDebug() << "Received avatar size: " << avatar.size();

        QStringList stringTokens;
        QString uniqueID;

        sentData.remove(0, 10); // for 'avatar of '
        sentData.remove(0, 1); // for open brace '{'
        sentData.remove(36, 1); // for closed brace '}'

        QString data(sentData);
        stringTokens = data.split('_');  //can be removed
        uniqueID = stringTokens.at(0);

        sentData.remove(0, 37); // for QUuid delete

        dm->setAvatarOfNextOnlineUser(avatar, QUuid(uniqueID));

    }

    // READING DOESN'T WORK PROPERLY YET
    // Probably the data that is sent (the avatar) is too big and a while 'waitForReadyRead' is needed.
    // Also a better solution would probably be to use signals and slots
    /*tcpSocket.waitForReadyRead(5000);
    sentData = tcpSocket.readAll();

    if(sentData.startsWith("avatar of ")){
        qDebug() << "Fortune Server: Avatar received (separate thread)";
        qDebug() << "Received data size: " << sentData.size();
        //tcpSocket.disconnectFromHost();


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
        buffer->open(QIODevice::ReadWrite);
        buffer->write(sentData);
        buffer->seek(0);

        QPixmap image;
        QImage image_2;
        QImage tryIm = QImage::fromData(sentData, "PNG");
        //uint size = sentData.size();
        image.loadFromData(sentData);
        image.loadFromData(buffer->buffer());
        image_2.loadFromData(buffer->buffer());


    }*/
}

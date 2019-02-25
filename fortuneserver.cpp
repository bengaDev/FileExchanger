#include "fortuneserver.h"
#include <QtConcurrent/QtConcurrent>

FortuneServer::FortuneServer(Data_Manager *dm, QObject *parent)
        : QTcpServer(parent)
{
    this->dm = dm;
}


void FortuneServer::incomingConnection(qintptr socketDescriptor){
    qDebug() << "Fortune Server: incoming connection (could be avatar or file)";
    QThread *receiverThread = new QThread;
    ReceiverWorker *receiver = new ReceiverWorker(dm, socketDescriptor);
    receiver->moveToThread(receiverThread);

    connect(receiverThread, SIGNAL (started()), receiver, SLOT (metadataStageSTART()));
    connect(receiver, SIGNAL (closeThread()), receiverThread, SLOT (quit()));
    connect(receiver, SIGNAL (closeThread()), receiver, SLOT (deleteLater()));
    connect(receiverThread, SIGNAL (closeThread()), receiverThread, SLOT (deleteLater()));
    receiverThread->start();
    //QtConcurrent::run(this, &FortuneServer::threadFunction, dm, socketDescriptor);
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
    qint64 msgSize = -1;
    qint64 temp = tcpSocket.bytesAvailable();

    if(temp && msgSize==-1){
        in >> msgSize;
    }

    in >> sentData;

qint64 receivedBytes = 0;
    if(sentData.startsWith("avatar of ")){

        while(tcpSocket.bytesAvailable() < msgSize - 8 ){
            qDebug() << "Bytes Available (to be read) : " << tcpSocket.bytesAvailable();
            if(!tcpSocket.waitForReadyRead(5000)){
                tcpSocket.disconnectFromHost();
                break;
            }
        }

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
    else if(sentData.startsWith("incoming file from ")){
        qint64 fileSize = msgSize;
        QByteArray fileBuffer;
        QString fileName;

        QString uniqueID;
        QString data(sentData);
        data.remove("incoming file from {");
        data.remove("}");
        uniqueID = data;

        in >> fileName;

        QFile file("./" + fileName);
        qDebug() << "-------------File name is: " << fileName;

        file.open(QIODevice::WriteOnly);

        // Keep reading from 'tcpSocket' untill all the bytes have been received
        while(receivedBytes < fileSize){

            // Wait untill incoming data amounts to >= 'PayloadSize' Bytes
            while(tcpSocket.bytesAvailable() < 64*1024){
                // If waiting for more than 5 seconds, exit the inner 'while'
                // and check if this waiting is due to end of transmission (receivedBytes>fileSize)
                // or if it's just because of poor connection, in which case the program will
                // re-enter in this while
                if(!tcpSocket.waitForReadyRead(5000)){
                    break;
                }
            }  //64Kb are arrived now...
            receivedBytes += tcpSocket.bytesAvailable();

            fileBuffer = tcpSocket.readAll();

            file.write(fileBuffer);
        }

        file.write(fileBuffer);

        file.close();



        /* =============== DOESN'T COMPLETELY WORK
         * while((receivedBytes = tcpSocket.bytesAvailable()) < fileSize  ){
            qDebug() << "Bytes Available (to be read) : " << tcpSocket.bytesAvailable();


            if(!tcpSocket.waitForReadyRead(5000)){
                qDebug() << "ERROR";
                //tcpSocket.disconnectFromHost();
                //break;
            }

        }
        if(receivedBytes >= fileSize){
            in >> buffer;
            fileMp3.append(buffer);
            qint64 temp = buffer.size();
            buffer.clear();
            in >> buffer;
            qint64 temp1 = buffer.size();
            tcpSocket.close();
        }*/


    }


}

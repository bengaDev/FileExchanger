#include "receiverworker.h"

ReceiverWorker::ReceiverWorker(Data_Manager* dm, qintptr socketDescriptor)
{
    this->dm = dm;

    tcpSocket = new QTcpSocket(this);

    if (!tcpSocket->setSocketDescriptor(socketDescriptor)) {
        //emit error(tcpSocket.error());
        qDebug() << "ReceiverWorker: error in opening socket thorugh 'socketDescriptor' - Threaded";
        return;
    }

    connect(dm, SIGNAL(messageBoxYes()), this, SLOT(dataStageSTART()), Qt::QueuedConnection);
}

void ReceiverWorker::metadataStageSTART(){
    qDebug() << "ReceiverWorker with thread: " << QThread::currentThreadId();

    tcpSocket->waitForReadyRead(5000);

    QDataStream in(tcpSocket);    

    if(tcpSocket->bytesAvailable() && msgSize == -1){
        in >> msgSize;
    }

    in >> sentData;

    if(sentData.startsWith("avatar of")){

        while(tcpSocket->bytesAvailable() < msgSize - 8){
            qDebug() << "Bytes Available (to be read) : " << tcpSocket->bytesAvailable();
            if(!tcpSocket->waitForReadyRead(5000)){
                tcpSocket->disconnectFromHost();
                break;
            }
        }

        qDebug() << "RecaiverWorker: Avatar received ( separate thread )";
        qDebug() << "Received data size: " << sentData.size();

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

        // At this point avatar is received, and thread should close.
        // In order to do this emit signal 'closeThread'

        emit closeThread();
    }
    else if(sentData.startsWith("incoming file from ")){
        fileSize = msgSize;

        QString senderName;
        QString data(sentData);
        data.remove("incoming file from {");
        data.remove("}");
        uniqueID = data;

        in >> fileName;
        in >> senderName;

        // END OF THIS PHASE (READING OF METADATA)
        emit dm->metadataStageEND(fileSize, fileName, QUuid(uniqueID), senderName);
    }

}

void ReceiverWorker::dataStageSTART(){

    tcpSocket->write("YES");
    tcpSocket->waitForBytesWritten(5000);

    QByteArray fileBuffer;
    QFile file("./" + fileName);
    qDebug() << "-------------File name is: " << fileName;

    file.open(QIODevice::WriteOnly);

    emit dm->setProgBarMaximum_RECEIVER(uniqueID, fileSize);

    // Keep reading from 'tcpSocket' untill all the bytes have been received
    while(receivedBytes < fileSize){

        // Wait untill incoming data amounts to >= 'PayloadSize' Bytes
        while(tcpSocket->bytesAvailable() < 64*1024){
            // If waiting for more than 5 seconds, exit the inner 'while'
            // and check if this waiting is due to end of transmission (receivedBytes>fileSize)
            // or if it's just because of poor connection, in which case the program will
            // re-enter in this while
            if(!tcpSocket->waitForReadyRead(5000)){
                break;
            }
        }  //64Kb are arrived now...
        receivedBytes += tcpSocket->bytesAvailable();

        emit dm->setProgBarValue_RECEIVER(uniqueID, receivedBytes);

        fileBuffer = tcpSocket->readAll();

        file.write(fileBuffer);
    }

    emit dm->setProgBarValue_RECEIVER(uniqueID, receivedBytes);

    file.close();

    // At this point file is received, and thread should close.
    // In order to do this emit signal 'closeThread'

    emit closeThread();
}

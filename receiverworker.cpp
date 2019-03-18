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

    //connect(dm, SIGNAL(messageBoxYes()), this, SLOT(dataStageSTART()), Qt::QueuedConnection);
    connect(dm, SIGNAL(savingPath(QString)), this, SLOT(pathSelectionSTART(QString)), Qt::QueuedConnection);
    connect(dm, SIGNAL(closeSocket()), this, SLOT(closeConnection()), Qt::QueuedConnection);

    connect(dm, SIGNAL(interruptReceiving(QUuid)), this, SLOT(onInterruptReceiving(QUuid)), Qt::QueuedConnection);
    connect(dm, SIGNAL(interruptAllReceiving()), this, SLOT(onInterruptAllReceiving()), Qt::QueuedConnection);

    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
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

void ReceiverWorker::pathSelectionSTART(QString path){
    QString uniqueFileName;

    if(dm->getIFDefaultSavingPath() == true){
        uniqueFileName = getCorrectFileName( QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) );
        file = new QFile( uniqueFileName );
    }
    else{
        uniqueFileName = getCorrectFileName(path);
        file = new QFile( uniqueFileName );
    }
    dataStageSTART();

}

QString ReceiverWorker::getCorrectFileName(QString path){
    QString completeFileName = path + "/" + fileName;
    QFileInfo tmpFile(path + "/" + fileName);

    if(tmpFile.exists()){
        // change fileName in file(counter).extension
        uint counter = 0;
        QString tmpBaseName = tmpFile.completeBaseName();
        while(tmpFile.exists()){
            counter += 1;
            completeFileName = tmpFile.path() + "/" + tmpBaseName + "(" + QString::number(counter) + ")." + tmpFile.completeSuffix();
            tmpFile = QFileInfo(completeFileName);
        }
        fileName = tmpFile.fileName();
    }
    return completeFileName;
}

void ReceiverWorker::dataStageSTART(){

    emit dm->setLabelProgBarWindow(uniqueID, "Receiving \"" + fileName + "\" from " + senderName);

    tcpSocket->write("YES");
    tcpSocket->waitForBytesWritten(30000);

    QByteArray fileBuffer;

    qDebug() << "-------------File name is: " << fileName;

    if(file->open(QIODevice::WriteOnly)== false){
        qDebug() << "File not opened correctly - RECEIVER";
    }

    in = new QDataStream(tcpSocket);
    emit dm->setProgBarMaximum_RECEIVER(uniqueID, fileSize);

    QMetaObject::invokeMethod(this, "receivingStep", Qt::QueuedConnection);
/*
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

        file->write(fileBuffer);
    }

    emit dm->setProgBarValue_RECEIVER(uniqueID, receivedBytes);

    file->close();

    // At this point file is received, and thread should close.
    // In order to do this emit signal 'closeThread'

    emit closeThread();
*/
}

/*void ReceiverWorker::receivingStep(){
    if(atomicFlag == 1){
        return;
    }

    // Keep reading from 'tcpSocket' untill all the bytes have been received
    if(receivedBytes < fileSize){
        fileBuffer.clear();


        while(tcpSocket->bytesAvailable() < PAYLOAD_SIZE && (tcpSocket->bytesAvailable() + receivedBytes) != (fileSize + 4)){ //4 bytes for data stream separator
            // If waiting for more than 5 seconds, exit the inner 'while'
            // and check if this waiting is due to end of transmission (receivedBytes>fileSize)
            // or if it's just because of poor connection, in which case the program will
            // re-enter in this while
            if(!tcpSocket->waitForReadyRead(10000) ){
                qDebug() << "----------------receiverWorker waited more than 10sec";
                qDebug() << "---------------- bytes Available " << tcpSocket->bytesAvailable();

                on_disconnected();

                return;
            }
        }  //64Kb are arrived now...

        *in >> fileBuffer;

        receivedBytes += fileBuffer.size();
        file->write(fileBuffer);

        emit dm->setProgBarValue_RECEIVER(uniqueID, receivedBytes);


        QMetaObject::invokeMethod(this, "receivingStep", Qt::QueuedConnection);
    }else{
        emit dm->setProgBarValue_RECEIVER(uniqueID, receivedBytes);

        if(file->isOpen()){
            file->close();
        }

        // At this point file is received, and thread should close.
        // In order to do this emit signal 'closeThread'

        tcpSocket->write("ACK");
        if(!tcpSocket->waitForBytesWritten(5000)){
            qDebug() << "Receiver Worker: ERROR -> ACK not sended!";
        }

        emit dm->setLabelProgBarWindow(uniqueID, "\"" + fileName + "\" received!");

        closingThread();
    }

}
*/
void ReceiverWorker::receivingStep(){
    if(atomicFlag == 1){
        return;
    }

    // Keep reading from 'tcpSocket' untill all the bytes have been received
    if(receivedBytes < fileSize){
        fileBuffer.clear();

        // Wait untill incoming data amounts to >= 'PayloadSize' Bytes
        while(tcpSocket->bytesAvailable() < PAYLOAD_SIZE && (tcpSocket->bytesAvailable() + receivedBytes) != (fileSize + 4)){
            // If waiting for more than 5 seconds, exit the inner 'while'
            // and check if this waiting is due to end of transmission (receivedBytes>fileSize)
            // or if it's just because of poor connection, in which case the program will
            // re-enter in this while
            if(!tcpSocket->waitForReadyRead(10000)){
                on_disconnected();

                return;
            }
        }  //64Kb are arrived now...
        receivedBytes += tcpSocket->bytesAvailable();

        emit dm->setProgBarValue_RECEIVER(uniqueID, receivedBytes);

        fileBuffer = tcpSocket->readAll();

        file->write(fileBuffer);

        QMetaObject::invokeMethod(this, "receivingStep", Qt::QueuedConnection);
    }else{
        emit dm->setProgBarValue_RECEIVER(uniqueID, receivedBytes);

        if(file->isOpen()){
            file->close();
        }

        // At this point file is received, and thread should close.
        // In order to do this emit signal 'closeThread'

        tcpSocket->write("ACK");
        if(!tcpSocket->waitForBytesWritten(5000)){
            qDebug() << "Receiver Worker: ERROR -> ACK not sended!";
        }

        emit closeThread();
    }

}

void ReceiverWorker::onInterruptAllReceiving(){
    onInterruptReceiving(this->uniqueID);
}

void ReceiverWorker::onInterruptReceiving(QUuid id){
    qDebug() << "SenderWorker: onInterruptReceived-> " + id.toString();
    QUuid this_id = QUuid(this->uniqueID);

    if(this_id == id){
        atomicFlag = 1;
        if(file->isOpen()){
            file->remove();
            emit dm->setLabelProgBarWindow(id, "Transfer from " + senderName + " was canceled by user");
        }
        closeConnection();
    }
}

void ReceiverWorker::on_disconnected(){
    qDebug() << "ReceiverWorker: SOCKET DISCONNECTED!";
    //close window

    if(file != nullptr && file->isOpen()){
        file->remove();
        emit dm->setLabelProgBarWindow(uniqueID, senderName + " interrupted transfer, or connection lost");
    }

    atomicFlag = 1;

    closingThread();
}

void ReceiverWorker::closeConnection(){
    qDebug() << "ReceiverWorker: close Connection!";
    closingThread();
}

void ReceiverWorker::closingThread(){
    if(tcpSocket->state() != QAbstractSocket::ClosingState){
        //tcpSocket->close();
        qDebug() << "SenderWorker: Socket now closed!!";
    }else{
        qDebug() << "SenderWorker: Socket ALREADY closed!!";
    }
    emit closeThread();
}

ReceiverWorker::~ReceiverWorker(){
    //disconnect(this, 0, 0, 0);
    this->disconnect();
}

#include "senderworker.h"

SenderWorker::SenderWorker(Data_Manager* dm,QUuid id, QHostAddress addr, QString nameSendingTo)
{
    this->dm = dm;
    this->uniqueID = id;
    this->nameSendingTo = nameSendingTo;
    this->filePath = dm->getFilePath();
    this->fileName = dm->getFileName();

    tcpSocket = new QTcpSocket(this);

    qDebug() << "SenderWorker: unique id =  " << id ;

    tcpSocket->connectToHost(addr, SERVER_PORT);

    qDebug() << "SenderWorker: socket state = " << tcpSocket->state() << " ( thread id: "
             << QThread::currentThreadId() << " )";

    //add connects to send meta-data and file
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(checkResponse()));

    //add connect to close socket
    connect(dm, SIGNAL(interruptSending(QUuid)), this, SLOT(onInterruptSending(QUuid)), Qt::QueuedConnection);
    connect(dm, SIGNAL(interruptAllSending()), this, SLOT(onInterruptAllSending()), Qt::QueuedConnection);

    //connect(tcpSocket, SIGNAL(stateChanged()), this, SLOT(DEBUG_socketStateChanged()));

    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(on_disconnected()));

}


void SenderWorker::sendMetaData(){
    qDebug() << "SenderWorker: sending Meta-Data";

    file = new QFile(filePath);

    if(file->open(QIODevice::ReadOnly) == false){
        qDebug() << "SenderWorker: " << "Error opening File";
        emit dm->fileOpenError();
        closingThread();
    }

    TotalBytes = file->size();

    qDebug() << "---------------File Size: " << TotalBytes;

    QByteArray firstPacketPayload;  //meta-data packet
    QDataStream out(&firstPacketPayload, QIODevice::ReadWrite);

    out << TotalBytes; // Size of file
    out << "incoming file from " + dm->localHost->getUniqueID().toByteArray();
    out << fileName;
    out << dm->localHost->getName();

    // Send first packet: metadata
    if(tcpSocket->write(firstPacketPayload) < firstPacketPayload.size()){
        qDebug("Transmission error in sending file");
    }
    tcpSocket->waitForBytesWritten(5000);

    // Each time something new is inserted in a DataStream, an automatic separator of 4 bytes is added
    // 'firstPacketPayload.size()' already has the first separator included
    // we are adding +4 bytes for the next separator


    // Set Max Value in corresponding ProgressBar of host 'h'
    emit dm->setProgBarMaximum_SENDER(uniqueID, TotalBytes);

    ////// WAIT FOR CLIENT TO ACCEPT REQUEST!//////
    //read on tcp socket -> receive ok from client
    // if tcpSocket.waitForReadyRead(5000) -> returns true ho ricevuto qualcosa
    // se client send ok vado avanti
    // se client send NOT ALLOWED annullo l'invio del file

}

void SenderWorker::checkResponse(){
    QByteArray response;

    response = tcpSocket->readAll();
    qDebug() << "SenderWorker: received response = " << response;

    if(response == "YES"){
        sendFile();
    }else if(response == "ACK"){
        qDebug() << "SenderWorker: Received ACK, closing thread";
        emit dm->setLabelProgBarWindow(uniqueID, "\"" + fileName + "\" sended!");
        closingThread();
    }
}

///called by signal readyRead
void SenderWorker::sendFile(){


    qDebug() << "SenderWorker: start sending file";
    emit dm->setLabelProgBarWindow(uniqueID, "Sending \"" + fileName + "\" to " + nameSendingTo);

    tcpStream = new QDataStream(tcpSocket);

    filePartition = file->read(FILE_PARTITION_SIZE);
    qDebug() << "File partition size: " << filePartition.size();
    QMetaObject::invokeMethod(this, "sendingStep", Qt::DirectConnection);

    if(TotalBytes == bytesWritten){

        if(file->isOpen()){
            file->close();
        }

        qDebug() << "SenderWorker: file sendend!";
    }



    /*
    // Read part of file and fill first 64KB TCP packet
    qint64 bytesWritten = 0;
    QByteArray buffer;

    // NOW  CONNECTION IS ESTABLISHED AND FIRST PACKET IS SENT
    // -> SEND ALL OF THE REMAINING PACKETS IN CASE FILE > 64KB
    while(!file->atEnd()){

        buffer.clear();

        // If the number of bytes waiting to be written is > 4*PayloadSize
        //      then don't write anything, and wait untill this number decreases
        //      to start writing again
        if(tcpSocket->bytesToWrite() <= 4*PayloadSize){

            buffer = file->read(PayloadSize);

            if((bytesWritten += tcpSocket->write(buffer)) < buffer.size()){
                qDebug("Transmission error in sending file");
            }
            if(!tcpSocket->waitForBytesWritten(5000)){
                qDebug() << "ERROR";
            }
        }
        // Update progress bar
        emit dm->setProgBarValue_SENDER(id, bytesWritten);
    }
    file->close();

    qDebug() << "SenderWorker: file sendend!";
    qDebug() << "--------------BytesWritten: " << bytesWritten ;

    // At this point file is sended, and thread should close.
    // In order to do this emit signal 'closeThread'
    emit closeThread();
    */
}

void SenderWorker::sendingStep(){

    if(atomicFlag == 1){
        // For interruptions
        return;
    }

    if(bytesWritten == TotalBytes){
        // File sended

        if(file->isOpen()){
            file->close();
        }
        return;
    }

    if(offsetPartition >= filePartition.size()){
        // File partition sended
        filePartition = file->read(FILE_PARTITION_SIZE);
        offsetPartition = 0;
    }

    // Read part of file and fill first 64KB TCP packet
    if(offsetPartition < filePartition.size()){
        buffer.clear();
        // If the number of bytes waiting to be written is > 4*PayloadSize
        //      then don't write anything, and wait untill this number decreases
        //      to start writing again


        buffer = filePartition.mid(offsetPartition, PAYLOAD_SIZE);

        *tcpStream << buffer;

        if(!tcpSocket->waitForBytesWritten(5000)){
            qDebug() << "ERROR";
        }

        bytesWritten += buffer.size();

        // Update progress bar
        emit dm->setProgBarValue_SENDER(uniqueID, bytesWritten);

        offsetPartition += PAYLOAD_SIZE;
        QMetaObject::invokeMethod(this, "sendingStep", Qt::QueuedConnection);

    }

    //qDebug() << "--------------BytesWritten: " << bytesWritten ;

    // At this point file is sended, and thread should close.
    // In order to do this emit signal 'closeThread'
    // before closing wait for Ack from receiver

    //closingThread();

}


/*void SenderWorker::sendingStep(){

    if(atomicFlag == 1){
        return;
    }

    if(bytesWritten == TotalBytes){
        return;
    }

    if(offsetPartition >= filePartition.size()){
        filePartition = file->read(FILE_PARTITION_SIZE);
        offsetPartition = 0;
    }

    // Read part of file and fill first 64KB TCP packet
    if(offsetPartition < filePartition.size()){
        buffer.clear();
        // If the number of bytes waiting to be written is > 4*PayloadSize
        //      then don't write anything, and wait untill this number decreases
        //      to start writing again
        if(tcpSocket->bytesToWrite() <= 10*PayloadSize){

            buffer = filePartition.mid(offsetPartition, PAYLOAD_SIZE);

            if((bytesWritten += tcpSocket->write(buffer)) < buffer.size()){
                qDebug("Transmission error in sending file");
            }
            if(!tcpSocket->waitForBytesWritten(5000)){
                qDebug() << "ERROR";
            }
        }
        // Update progress bar
        emit dm->setProgBarValue_SENDER(id, bytesWritten);

        offsetPartition += PAYLOAD_SIZE;
        QMetaObject::invokeMethod(this, "sendingStep", Qt::QueuedConnection);

    }

    //qDebug() << "--------------BytesWritten: " << bytesWritten ;

    // At this point file is sended, and thread should close.
    // In order to do this emit signal 'closeThread'
    // before closing wait for Ack from receiver

    //closingThread();

}*/


/*void SenderWorker::sendingStep(){

    if(atomicFlag == 1){
        return;
    }

    // Read part of file and fill first 64KB TCP packet
    if(!file->atEnd()){
        buffer.clear();
        // If the number of bytes waiting to be written is > 4*PayloadSize
        //      then don't write anything, and wait untill this number decreases
        //      to start writing again
        if(tcpSocket->bytesToWrite() <= 10*PayloadSize){

            buffer = file->read(PayloadSize);

            if((bytesWritten += tcpSocket->write(buffer)) < buffer.size()){
                qDebug("Transmission error in sending file");
            }
            if(!tcpSocket->waitForBytesWritten(5000)){
                qDebug() << "ERROR";
            }
        }
        // Update progress bar
        emit dm->setProgBarValue_SENDER(id, bytesWritten);

        QMetaObject::invokeMethod(this, "sendingStep", Qt::QueuedConnection);

    } else{

        if(file->isOpen()){
            file->close();
        }

        qDebug() << "SenderWorker: file sendend!";
        qDebug() << "--------------BytesWritten: " << bytesWritten ;

        // At this point file is sended, and thread should close.
        // In order to do this emit signal 'closeThread'
        // before closing wait for Ack from receiver

        //closingThread();
    }
}
*/

void SenderWorker::onInterruptAllSending(){
    onInterruptSending(this->uniqueID);
}

void SenderWorker::onInterruptSending(QUuid id){
    // cancel button pressed
    if(this->uniqueID == id){
        atomicFlag = 1;
        if(file->isOpen()){
            file->close();
            emit dm->setLabelProgBarWindow(id, "Transfer to " + nameSendingTo + " was canceled by user");
        }

        closeConnection();

    }
}


void SenderWorker::on_disconnected(){
    // socket disconnected
    qDebug() << "SenderWorker: SOCKET DISCONNECTED!";
    //close window 
    if(file->isOpen()){
        file->close();
        emit dm->setLabelProgBarWindow(uniqueID, nameSendingTo + " interrupted transfer, or connection lost");
    }

    atomicFlag = 1;

    //close thread
    closingThread();
}

void SenderWorker::closeConnection(){
    qDebug() << "SenderWorker: Closing socket!";

    if(tcpSocket->state() == QAbstractSocket::ClosingState){
        qDebug() << "SenderWorker: Socket ALREADY closed!!";
    }

    closingThread();
}

void SenderWorker::closingThread(){
    tcpSocket->close();
    emit dm->endSendingFile(filePath); // For deleting zip file of directory
    emit closeThread();
}


void SenderWorker::DEBUG_socketStateChanged(){
    qDebug() << "SenderWorker: SOCKET STATE CHANGED!";

}

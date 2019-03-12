#include "senderworker.h"

SenderWorker::SenderWorker(Data_Manager* dm,QUuid id, QHostAddress addr)
{
    this->dm = dm;
    this->id = id;

    tcpSocket = new QTcpSocket(this);

    qDebug() << "SenderWorker: unique id =  " << id ;

    tcpSocket->connectToHost(addr, SERVER_PORT);

    qDebug() << "SenderWorker: socket state = " << tcpSocket->state() << " ( thread id: "
             << QThread::currentThreadId() << " )";

    //add connects to send meta-data and file
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(checkResponse()));

    //add connect to close socket
    connect(dm, SIGNAL(interruptSending(QUuid)), this, SLOT(onInterruptSending(QUuid)), Qt::QueuedConnection);

    //connect(tcpSocket, SIGNAL(stateChanged()), this, SLOT(DEBUG_socketStateChanged()));

    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
}


void SenderWorker::sendMetaData(){
    qDebug() << "SenderWorker: sending Meta-Data";

    file = new QFile(dm->getFilePath());

    if(file->open(QIODevice::ReadOnly) == false){
        qDebug() << "SenderWorker: " << "Error opening File";
    }

    qint64 TotalBytes = file->size();

    qDebug() << "---------------File Size: " << TotalBytes;

    QByteArray firstPacketPayload;  //meta-data packet
    QDataStream out(&firstPacketPayload, QIODevice::ReadWrite);

    out << TotalBytes; // Size of file
    out << "incoming file from " + dm->localHost->getUniqueID().toByteArray();
    out << dm->getFileName();
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
    emit dm->setProgBarMaximum_SENDER(id, TotalBytes);

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
    }else{
        qDebug() << "SenderWorker: closing connection";
    }
}

///called by signal readyRead
void SenderWorker::sendFile(){
    qDebug() << "SenderWorker: start sending file";
    QMetaObject::invokeMethod(this, "sendingStep", Qt::QueuedConnection);
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
        return;
    }

    // Read part of file and fill first 64KB TCP packet
    if(!file->atEnd()){
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

        QMetaObject::invokeMethod(this, "sendingStep", Qt::QueuedConnection);

    } else{

        if(file->isOpen()){
            file->close();
        }

        qDebug() << "SenderWorker: file sendend!";
        qDebug() << "--------------BytesWritten: " << bytesWritten ;

        // At this point file is sended, and thread should close.
        // In order to do this emit signal 'closeThread'
        emit closeThread();
    }
}


void SenderWorker::onInterruptSending(QUuid id){
    //qDebug() << "SenderWorker: onInterruptSending-> " + id.toString();
    if(this->id == id){
        atomicFlag = 1;
        if(file->isOpen()){
            file->close();
        }

        closeConnection();

    }
}


void SenderWorker::on_disconnected(){
    qDebug() << "SenderWorker: SOCKET DISCONNECTED!";
    //close window

    if(file->isOpen()){
        file->close();
    }

    //close thread
    emit closeThread();
}

void SenderWorker::closeConnection(){
    qDebug() << "SenderWorker: Closing socket!";

    if(tcpSocket->state() == QAbstractSocket::ClosingState){
        qDebug() << "SenderWorker: Socket ALREADY closed!!";
    }
    tcpSocket->close(); // automatically calls tcpSocket->disconnectFromHost();

    emit closeThread();
}


void SenderWorker::DEBUG_socketStateChanged(){
    qDebug() << "SenderWorker: SOCKET STATE CHANGED!";

}

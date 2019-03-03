#include "senderworker.h"

SenderWorker::SenderWorker(Data_Manager* dm, Host* h)
{
    this->dm = dm;
    this->h = h;
    tcpSocket = new QTcpSocket(this);

    tcpSocket->connectToHost(h->getIP(), SERVER_PORT);
    qDebug() << "SenderWorker: socket state = " << tcpSocket->state() << " ( thread id: "
             << QThread::currentThreadId() << " )";

    //add connects to send meta-data and file

 /*
    if (!tcpSocket->setSocketDescriptor(socketDescriptor)) {
        //emit error(tcpSocket.error());
        qDebug() << "SenderWorker: error in opening socket thorugh 'socketDescriptor' - Threaded";
        return;
    }
*/

}


void SenderWorker::sendMetaData(){
    file = dm->getFileToSend();

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
    emit dm->setProgBarMaximum_SENDER(h.getUniqueID(), TotalBytes);

    ////// WAIT FOR CLIENT TO ACCEPT REQUEST!//////
    //read on tcp socket -> receive ok from client
    // if tcpSocket.waitForReadyRead(5000) -> returns true ho ricevuto qualcosa
    // se client send ok vado avanti
    // se client send NOT ALLOWED annullo l'invio del file

///TO BE RE-IMPLEMENTED WITH SIGNALS
/*
    if(tcpSocket.waitForReadyRead(10000)){
        QByteArray response = tcpSocket.readAll();
        if(response == "YES"){
            sendingFile(&tcpSocket, file, h);
        }
    }
*/

}


void SenderWorker::sendFile(){
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
        emit dm->setProgBarValue_SENDER(h.getUniqueID(), bytesWritten);
    }
    qDebug() << "--------------BytesWritten: " << bytesWritten ;
}

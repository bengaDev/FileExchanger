#include "client.h"

/*
 * CHECK IF SOMETHING GOES WRONG!
 *
 * maybe a slot that manages error() signal is needed
 * or a try catch
 *
 * check how many sockets will be created in case of ANY connection
 *
*/

static const int PayloadSize = 1 * 1024; // 64 KB

Client::Client(Data_Manager *dm, QObject *parent) :
    QObject (parent)
{
    this->dm = dm;
    qDebug() << "Client: Hello i'm " << dm->localHost->getName() <<
                "and my visibility is: " << dm->localHost->isVisible();
    qDebug() << "Client: " << dm->localHost->getUniqueID();

    udpSocket = new QUdpSocket(this);
    //tcpSocket = new QTcpSocket(this);

    if(!udpSocket->bind(BROADCAST_PORT, QUdpSocket::ShareAddress)){
        // UDP listener of server could not start
        qDebug() << "Client: UDP listener could not start!";
    } else {
        // UDP listener started
        qDebug() << "Client: UDP listener started!";
    }

    broadcastAddress = findBroadcastAddress();

    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(on_UdpReceive()));

    connect(dm, SIGNAL(sendFile_SIGNAL()), this, SLOT(sendFile()));

    connect(dm, SIGNAL(quittingApplication()), this, SLOT(onQuittingApplication()));

    QtConcurrent::run(this, &Client::hello);


}

void Client::hello(){
    qDebug() << "Client: inizialization...";
    uint refreshTime = dm->getRefreshTime();
    QUdpSocket udpSocket;

    while(atomicLoopFlag == 1){
        QByteArray datagram = "HOST Basic Info:" + dm->localHost->getUniqueID().toByteArray()
                + "_" + dm->localHost->getName().toUtf8() + "_" + dm->localHost->getVisibilityStatus().toUtf8();
        qDebug() << "=======================================================\n";
        qDebug() << "Client: Broadcasting basic info -- UDP";


        if(udpSocket.writeDatagram(datagram, broadcastAddress, SERVER_PORT) == -1){

            qDebug() << "Client: Could not send broadcast basic info -- UDP";
        } else {
            qDebug() << "Client: Broadcast of basic info done -- UDP" << udpSocket.state();

        }

        //Refresh datamanager
        dm->refreshOnlineUsers();

        QThread::sleep(refreshTime);

    }

    /*
    // when deleting client annunce visible = false
    QByteArray datagram = "HOST Basic Info:" + dm->localHost->getUniqueID().toByteArray()
            + "_" + dm->localHost->getName().toUtf8() + "_" + QString("NOTVISIBLE").toUtf8();
    qDebug() << "=======================================================\n";
    qDebug() << "Client: Sendig last message -- UDP";


    if(udpSocket.writeDatagram(datagram, QHostAddress("192.168.1.255"), SERVER_PORT) == -1){

        qDebug() << "Client: Could not send last message -- UDP";
    } else {
        qDebug() << "Client: Last message sended -- UDP" << udpSocket.state();

    }
    */
}

void Client::on_UdpReceive(){
    QByteArray datagram;
    QHostAddress senderIP;
    QString datagramString;

    qDebug() << "Client: on_udpRecaived!";

    while (udpSocket->hasPendingDatagrams()) {
        datagram.resize(int(udpSocket->pendingDatagramSize()));
        udpSocket->readDatagram(datagram.data(), datagram.size(), &senderIP);

        datagramString = QString(datagram);

        if(datagram.startsWith("SERVER REQUEST more info")){
            // I want to send avatar on separate thread
            //sendAvatar(dm, senderIP);
            qDebug() << "Client: received request for more info -- UDP";
            QtConcurrent::run(this, &Client::sendAvatar, senderIP);
        }
    }


}

void Client::sendAvatar(QHostAddress senderIP){

    //QImage avatar(dm->localHost->getAvatar().toImage());

    QPixmap avatar(dm->localHost->getAvatar());
    QTcpSocket tcpSocket;
    tcpSocket.connectToHost(senderIP, SERVER_PORT);
    QByteArray buffer;

    ///QFile file;
    ///buffer = file.read(1024); to send a file

    QDataStream out(&buffer, QIODevice::ReadWrite);

    out << qint64(0); // This inserts in buffer an int (8 bytes), where later the size will be stored
    out << "avatar of " + dm->localHost->getUniqueID().toByteArray();
    out << avatar; // binary representation of the image

    out.device()->seek(0);
    out << buffer.size(); // Here size of int(0)+avatar is inserted in place of int(0)

    qDebug() << "Client: sending avatar (separate thread)-- TCP";
    qDebug() << "Avatar size: " << buffer.size();

    if(tcpSocket.write(buffer) < buffer.size()){
        qDebug("Transmission error in sending avatar");
    }
    tcpSocket.waitForBytesWritten(5000);

    /*QByteArray pictureBin;
    QBuffer buffer(&pictureBin);
    avatar.save(&buffer, "PNG");

    QTcpSocket tcpSocket;
    tcpSocket.connectToHost(senderIP, 1515);

    qDebug() << "Client: sending avatar (separate thread)-- TCP";
    qDebug() << "Avatar size: " << pictureBin.size();

    //tcpSocket.write("avatar");
    tcpSocket.write("avatar of " + dm->localHost->getUniqueID().toByteArray() + "_" + pictureBin);
    tcpSocket.waitForBytesWritten(5000);*/


}

void Client::sendFile(){

    std::list<Host> toSendUsers = dm->getToSendUsers();

    if(!toSendUsers.empty()){
        for(std::list<Host>::iterator it = toSendUsers.begin(); it != toSendUsers.end(); it++){
            qDebug() << "CLient: starting a new worker";

            ///CREATES A NEW SENDER WORKER FOR EACH USER///
            QThread *senderThread = new QThread;
            SenderWorker *senderWorker = new SenderWorker(dm, it->getUniqueID(), it->getIP());
            senderWorker->moveToThread(senderThread);

            //add connects to manage thread closing
            connect(senderThread, SIGNAL(started()), senderWorker, SLOT(sendMetaData()));
            connect(senderWorker, SIGNAL(closeThread()), senderThread, SLOT(quit()));
            connect(senderWorker, SIGNAL(closeThread()), senderWorker, SLOT(deleteLater()));
            connect(senderThread, SIGNAL(finished()), senderThread, SLOT(deleteLater()));

            senderThread->start();

        }
        dm->deleteAllToSendUsers();
    }
}

void Client::sendMetadataToUser(Host h){
    // -------------------------
    // INSIDE SEPARATE THREAD
    // -------------------------

    QFile* file = dm->getFileToSend();
    if(file->open(QIODevice::ReadOnly) == false){
        qDebug() << "Error opening File";
    }
    
    QTcpSocket tcpSocket;
    tcpSocket.connectToHost(h.getIP(), SERVER_PORT);
    qDebug() << tcpSocket.state();


    qint64 TotalBytes = file->size();

    qDebug() << "---------------File Size: " << TotalBytes;

    QByteArray firstPacketPayload;  //meta-data packet
    QDataStream out(&firstPacketPayload, QIODevice::ReadWrite);

    out << TotalBytes; // Size of file
    out << "incoming file from " + dm->localHost->getUniqueID().toByteArray();
    out << dm->getFileName();
    out << dm->localHost->getName();

    // Send first packet: metadata
    if(tcpSocket.write(firstPacketPayload) < firstPacketPayload.size()){
        qDebug("Transmission error in sending file");
    }
    tcpSocket.waitForBytesWritten(5000);

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


    if(tcpSocket.waitForReadyRead(10000)){
        QByteArray response = tcpSocket.readAll();
        if(response == "YES"){
            sendingFile(&tcpSocket, file, h);
        }
    }


}

void Client::sendingFile(QTcpSocket* tcpSocket, QFile* file, Host h){
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

QHostAddress Client::findBroadcastAddress(){
    QHostAddress localHostAddress;
    QHostAddress broadcast = QHostAddress("0.0.0.0");
    QTcpSocket socket;

    socket.connectToHost("8.8.8.8", 53); // google DNS, or something else reliable

    if (socket.waitForConnected()) {
        qDebug()
            << "local IPv4 address for Internet connectivity is"
            << socket.localAddress();
            localHostAddress = socket.localAddress();
    } else {
        qWarning()
            << "could not determine local IPv4 address:"
            << socket.errorString();
    }

    foreach (const QNetworkInterface &interface, QNetworkInterface::allInterfaces()) {
        foreach(const QNetworkAddressEntry &address, interface.addressEntries()){
            /*
            if(address.ip().protocol() == QAbstractSocket::IPv4Protocol){
                qDebug() << "Address: " << address.ip().toString();
                qDebug() << "broadcast: " << address.broadcast().toString();
                qDebug() << "netmask: " << address.netmask().toString();
            }
            */
            if (address.ip() == localHostAddress){
                qDebug() << "---------- Broadcast: " << address.broadcast();
                broadcast = address.broadcast();
            }
        }
        //qDebug() << "______________________________________________";
    }


    return broadcast;
}

void Client::onQuittingApplication(){
    QUdpSocket udpSocket;

    // when deleting client annunce visible = false
    QByteArray datagram = "HOST Basic Info:" + dm->localHost->getUniqueID().toByteArray()
            + "_" + dm->localHost->getName().toUtf8() + "_" + QString("NOTVISIBLE").toUtf8();
    qDebug() << "=======================================================\n";
    qDebug() << "Client: Sendig last message -- UDP";

    if(udpSocket.writeDatagram(datagram, QHostAddress("192.168.1.255"), SERVER_PORT) == -1){

        qDebug() << "Client: Could not send last message -- UDP";
    } else {
        qDebug() << "Client: Last message sended -- UDP" << udpSocket.state();
    }
}

Client::~Client(){
    atomicLoopFlag = 0;
}

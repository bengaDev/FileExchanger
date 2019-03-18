#ifndef SENDERWORKER_H
#define SENDERWORKER_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QBuffer>
#include "data_manager.h"

class SenderWorker : public QObject
{
    Q_OBJECT
public:
    //explicit SenderWorker(Data_Manager* dm, Host h, QObject *parent = nullptr);
    SenderWorker(Data_Manager* dm, QUuid uniqueID, QHostAddress addr, QString nameSendingTo);

    void sendFile();

signals:
    void closeThread();

public slots:
    void checkResponse();
    void sendMetaData();

    void sendingStep();

    void onInterruptAllSending();
    void onInterruptSending(QUuid uniqueID);
    void DEBUG_socketStateChanged();

    void on_disconnected();
private:
    Data_Manager* dm;
    QUuid uniqueID;
    QString nameSendingTo;
    //QHostAddress addr;
    QTcpSocket* tcpSocket;

    QString filePath;
    QString fileName;
    QFile* file;
    QByteArray filePartition;
    int offsetPartition = 0;
    bool allPartitionsSent = false;
    //qint64 bytesWritten_Partitions = 0;
    static const int PayloadSize = 256 * 1024; // 64 KB

    qint64 TotalBytes = 0;
    qint64 bytesWritten = 0;
    QByteArray buffer;
    QAtomicInt atomicFlag = 0;

    QDataStream *tcpStream;

    void closeConnection();
    void closingThread();
};

#endif // SENDERWORKER_H

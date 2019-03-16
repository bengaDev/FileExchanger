#ifndef SENDERWORKER_H
#define SENDERWORKER_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include "data_manager.h"

class SenderWorker : public QObject
{
    Q_OBJECT
public:
    //explicit SenderWorker(Data_Manager* dm, Host h, QObject *parent = nullptr);
    SenderWorker(Data_Manager* dm, QUuid id, QHostAddress addr, QString nameSendingTo);

    void sendFile();

signals:
    void closeThread();

public slots:
    void checkResponse();
    void sendMetaData();

    void sendingStep();

    void onInterruptSending(QUuid id);
    void DEBUG_socketStateChanged();

    void on_disconnected();
private:
    Data_Manager* dm;
    QUuid id;
    QString nameSendingTo;
    //QHostAddress addr;
    QTcpSocket* tcpSocket;

    QString filePath;
    QFile* file;
    static const int PayloadSize = 1 * 1024; // 64 KB

    qint64 bytesWritten = 0;
    QByteArray buffer;
    QAtomicInt atomicFlag = 0;

    void closeConnection();
    void closingThread();
};

#endif // SENDERWORKER_H

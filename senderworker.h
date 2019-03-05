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
    SenderWorker(Data_Manager* dm, QUuid id, QHostAddress addr);

    void sendFile();

signals:
    void closeThread();

public slots:
    void checkResponse();
    void sendMetaData();
    void closeConnection();//to be connected with closing windowProgressBar

private:
    Data_Manager* dm;
    QUuid id;
    //QHostAddress addr;
    QTcpSocket* tcpSocket;

    QFile* file;
    static const int PayloadSize = 1 * 1024; // 64 KB

};

#endif // SENDERWORKER_H

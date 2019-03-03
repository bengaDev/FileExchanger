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
    //explicit SenderWorker(QObject *parent = nullptr);
    SenderWorker(Data_Manager* dm, Host* h);
    void sendMetaData();
    void sendFile();

signals:

public slots:

private:
    Data_Manager* dm;
    Host h;
    QTcpSocket* tcpSocket;

    QFile* file;
    static const int PayloadSize = 1 * 1024; // 64 KB

};

#endif // SENDERWORKER_H

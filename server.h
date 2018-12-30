#ifndef SERVER_H
#define SERVER_H
#define SERVER_PORT 1515

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <data_manager.h>


class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);

signals:

public slots:
    void newConnection();

private:
    QTcpServer *tcpServer;
    quint16 port = SERVER_PORT; //use this port or example
    Data_Manager *dm = nullptr;
};

#endif // SERVER_H

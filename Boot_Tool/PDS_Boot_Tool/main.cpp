#include <QCoreApplication>
#include <QUdpSocket>
#include <QFileInfo>
#include "JlCompress.h"
#include <QDebug>
#include <QByteArray>

void sendPath(QString path, QString isDir);

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);

    QString path = "C:/Users/nunxy/QT_ContextMenuAPP/contextMenuApp";
/*
    for (int i = 1; i < argc; ++i){
        path.append(QString(argv[i]));
        if(i < argc - 1){
            path.append(" ");
        }
    }
*/
    // Understand if path is directory or file
    QFileInfo outputPath(path);

    if(outputPath.isDir()){
        // ZIP directory and send path of zipped file to main app

        QString zipFile("./" + outputPath.baseName() + ".zip");

        if(JlCompress::compressDir(zipFile, path)){
            qDebug() << "Complete";
            sendPath(zipFile, "YES");
        }
        else {
            qDebug() << "Error";
        }
    }
    else {
        // send directly the path of the file
        sendPath(path, "NO");
    }


    return 0;// a.exec();
}

void sendPath(QString path, QString isDir){
    QUdpSocket udpSocket;
    QByteArray datagram = "BootTool isDir?" + isDir.toUtf8() + "?" + path.toUtf8();

    if(udpSocket.writeDatagram(datagram, QHostAddress::LocalHost, 2017) == -1){

        qDebug() << "Boot Tool: Could not send path -- UDP";
    } else {
        qDebug() << "Boot Tool: path sended -- UDP";

    }

}

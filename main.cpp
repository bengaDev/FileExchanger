#include "mainwindow.h"
#include "data_manager.h"
#include "server.h"
#include "client.h"
#include <QApplication>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QThread>
#include <QtConcurrent/QtConcurrent>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(systray);

    QApplication a(argc, argv);

    Data_Manager localDataManager("localHostName");

    Server myServer; //THIS STARTS THE SERVER

    QThread::currentThread()->setObjectName("Main Thread");

    Client myClient(&localDataManager);

    /*QThread t1;

    t1.setObjectName("Client Thread");

    QObject::connect(&t1, &QThread::started, &myClient, &Client::hello);
    QObject::connect(&t1, SIGNAL(finished()), &a, SLOT(quit()));  //probably this close the application when client is closed

    myClient.moveToThread(&t1);
    t1.start();  //Client starts*/

    //qDebug() << "Main Thread continue...";

    //myClient->moveToThread(QApplication::instance()->thread()); //moves an object to the main thread
    //note that you can’t call a method of an object in another thread directly so instead you use signals and slots

    MainWindow w(nullptr, &localDataManager);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QObject::tr("Systray"),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }


    // --------------------------------------------------------------------
    // This is used in order not to close application when window is closed
    // --------------------------------------------------------------------
    QApplication::setQuitOnLastWindowClosed(false);

    w.show();

    return a.exec();
}

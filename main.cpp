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
    Q_INIT_RESOURCE(systray_1);

    QApplication a(argc, argv);

    //QString fileName("C:/Users/Benjamin/Desktop/Poli/Prog_SISTEMA/PROGETTO/PDS_fileExchanger/Icon_IMG/I Feel Good.mp3");
    //QString fileName("C:/Users/Benjamin/Desktop/Poli/Prog_SISTEMA/PROGETTO/testFolder.rar");
    //QString fileName("C:/Users/nunxy/UNIVERSITA'/Magistrale/Programmazione Di Sistema/Progetto/code/PDS_fileExchanger/Icon_IMG/avatar_2.png");
    QString fileName("C:/Users/nunxy/Downloads/progetto2017.pdf");
    //QString fileName(":Icon_IMG/avatar_2.png");

    for (int i = 1; i < argc; ++i){
        //TO BE IMPLEMENTED IN FINAL VERSION
        //fileName = argv[i];
    }

    Data_Manager localDataManager("Nunzio", fileName);  //filename sarà null perchè non entro nel for(argc..)

    Server myServer(&localDataManager); //THIS STARTS THE SERVER

    QThread::currentThread()->setObjectName("Main Thread");

    Client myClient(&localDataManager);



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

    w.hide();
    //w.show();

    return a.exec();
}

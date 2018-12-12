#include "mainwindow.h"
#include "data_manager.h"
#include <QApplication>
#include <QSystemTrayIcon>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(systray);

    QApplication a(argc, argv);

    Data_Manager localDataManager("localHostName");

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

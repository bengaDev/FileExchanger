#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "data_manager.h"
#include "flowlayout.h"
#include "containergui.h"
#include "windowprogressbar.h"

#include <QMainWindow>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QLabel>
#include <QPainter>
#include <QMessageBox>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    void setVisible(bool visible) override;

protected:
    void closeEvent(QCloseEvent *event) override;

public:
    explicit MainWindow(QWidget *parent = nullptr, Data_Manager* dM = nullptr);
    ~MainWindow();

private slots:
    void updateAvatarVisible();
    void DEBUG_addToDataManager();
    void DEBUG_clearDataManager();
    void addUuidToSend_SLOT(QUuid);
    void deleteUuidToSend_SLOT(QUuid);
    void onShareButton();
    void messageBoxYES_NO(qint64, QString, QUuid, QString);

private:
    Ui::MainWindow *ui;
    void createTrayIcon();
    QPixmap maskPixMap(QPixmap pm);
    ContainerGUI* fromHost_to_Container(Host h);
    QList<ContainerGUI*> *onlineUsersGUI;
    QString avatarStyleSheet();
    void addReceiverProgBar(QUuid, QString);


    Data_Manager* dataManager;

    // QActions for system tray
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QMenu *trayIconMenu;
    QSystemTrayIcon *trayIcon;

    FlowLayout *flowLayout_ScrollArea;

    WindowProgressBar *sendingWindow;
    WindowProgressBar *receiverWindow;
};

#endif // MAINWINDOW_H

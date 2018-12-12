#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "data_manager.h"
#include "flowlayout.h"

#include <QMainWindow>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QLabel>


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

private:
    Ui::MainWindow *ui;
    void createTrayIcon();

    Data_Manager* dataManager;

    // QActions for system tray
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QMenu *trayIconMenu;
    QSystemTrayIcon *trayIcon;

    FlowLayout *flowLayout_ScrollArea;
};

#endif // MAINWINDOW_H

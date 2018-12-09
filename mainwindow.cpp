#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createTrayIcon();

    //connect(ui->BackBtn, SIGNAL(clicked()), this, SLOT());
    //connect(ui->ShareBtn, SIGNAL(clicked()), this, SLOT());

    QMainWindow::setVisible(true);
    QIcon *icon = new QIcon(":/Icon_IMG/shareIMG.png");
    setWindowTitle("Hello");
    if(icon != nullptr){
        setWindowIcon(QIcon(":/Icon_IMG/shareIMG.png"));
    }


}

void MainWindow::setVisible(bool visible){
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QMainWindow::setVisible(visible);
}

void MainWindow::closeEvent(QCloseEvent *event){
#ifdef Q_OS_OSX
    if(!event->spontaneous() || !isVisible()){
        return
    }
#endif
    if(trayIcon->isVisible()){

        this->hide();
        event->ignore();
    }


}

void MainWindow::createTrayIcon(){
    // Creating actions for system tray menu ----------------------------------------
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, &QAction::triggered, this, &QMainWindow::hide);

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, &QAction::triggered, this, &QMainWindow::showMaximized);

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, &QAction::triggered, this, &QMainWindow::showNormal);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, this, &QCoreApplication::quit);
    // -------------------------------------------------------------------------------

    // Create system tray menu -------------------------------------------------------
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/Icon_IMG/shareIMG.png"));
    trayIcon->setVisible(true);
    // -------------------------------------------------------------------------------
}

MainWindow::~MainWindow()
{
    delete ui;
}

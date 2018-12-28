#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent, Data_Manager* dM) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    dataManager(dM)
{
    ui->setupUi(this);
    onlineUsersGUI = new QList<ContainerGUI*>();
    createTrayIcon();

    //connect(ui->BackBtn, SIGNAL(clicked()), this, SLOT());
    //connect(ui->ShareBtn, SIGNAL(clicked()), this, SLOT());

    QMainWindow::setVisible(true);
    QIcon *icon = new QIcon(":/Icon_IMG/shareIMG.png");
    setWindowTitle("Hello");
    if(icon != nullptr){
        setWindowIcon(QIcon(":/Icon_IMG/shareIMG.png"));
    }

    QPixmap pix = dataManager->localHost->getAvatar();
    QPixmap maskedPixMap = maskPixMap(pix);
    flowLayout_ScrollArea = new FlowLayout();

    ui->toolButton->setIcon(QIcon(maskedPixMap));

    ui->BackBtn->setStyleSheet("QPushButton:hover{background-color: yellow}");

    // ============================

    ui->scrollArea->widget()->setLayout(flowLayout_ScrollArea);


    Host h1(true, "host1"); // = new Host(true, "host1");
    Host h2(true, "host2"); // = new Host(true, "host2");
    h1.createuniqueID();
    h2.createuniqueID();
    dataManager->addOnlineUser(h1);
    dataManager->addOnlineUser(h2);

    qDebug() << dataManager->getOnlineUsers().back().getName() << " : " << dataManager->getOnlineUsers().back().uniqueID;

    // Temp. connections just to simulate server updates
    connect(ui->ShareBtn, SIGNAL(clicked()), this, SLOT(DEBUG_addToDataManager()));
    connect(ui->BackBtn, SIGNAL(clicked()), this, SLOT(DEBUG_clearDataManager()));

    // When a list in dataManager is updated, it triggers this SLOT, which will update the GUI
    connect(dataManager, SIGNAL(isUpdated()), this, SLOT(updateAvatarVisible()));

    dataManager->deleteOnlineUser(h1);

}

void MainWindow::DEBUG_addToDataManager(){
    Host temp = *(dataManager->localHost);
    dataManager->addOnlineUser(temp);
}
void MainWindow::DEBUG_clearDataManager(){
    dataManager->DEBUG_clearOnlineUsers();
    ui->ListCounter->setText(QString::number(onlineUsersGUI->size()));
}

// This SLOT will be called when 'onlineUsers' in 'dataManager' is updated /
// when the SIGNAL 'isUpdated()' is called
void MainWindow::updateAvatarVisible(){

    std::list<Host> l = dataManager->getOnlineUsers();

    // First empty the current widgets from GUI Layout
    for(int i = 0; i < onlineUsersGUI->size(); i++){
        QWidget *lastItem = onlineUsersGUI->at(i);

        flowLayout_ScrollArea->removeWidget(lastItem);
        delete lastItem;
    }

    // Empty the list of pointers to those widgets
    onlineUsersGUI->clear();


    // Then fill it back updated
    for(std::list<Host>::iterator it=l.begin(); it != l.end(); it++){
        ContainerGUI *container = fromHost_to_Container(*it);
        onlineUsersGUI->push_back(container);

        flowLayout_ScrollArea->addWidget(onlineUsersGUI->last());

        ui->ListCounter->setText(QString::number(onlineUsersGUI->size()));
    }


}

ContainerGUI* MainWindow::fromHost_to_Container(Host h){
    QPixmap pix = h.getAvatar();
    QPixmap maskedPixmap = maskPixMap(pix);

    ContainerGUI *container = new ContainerGUI(nullptr, maskedPixmap);

    container->setAvatarPixmap(maskedPixmap);
    container->setUniqueID(h.getUniqueID());
    qDebug() << "Container - " << h.getName() << " : " << container->uniqueId;

    container->setAvatarLabel(h.getName());

    connect(container, SIGNAL(addUuidToSend_SIGNAL(QUuid)), this, SLOT(addUuidToSend_SLOT(QUuid)));
    connect(container, SIGNAL(deleteUuidToSend_SIGNAL(QUuid)), this, SLOT(deleteUuidToSend_SLOT(QUuid)));

    return container;

}

void MainWindow::addUuidToSend_SLOT(QUuid ID){
    dataManager->addToSendUsers(ID);
    ui->ListCounter->setText(QString::number(ui->ListCounter->text().toInt()+1));
}

void MainWindow::deleteUuidToSend_SLOT(QUuid ID){
    dataManager->deleteToSendUsers(ID);
}

QString MainWindow::avatarStyleSheet(){
    return "QToolButton{border: 1px;"
            "width: 100px; min-width: 100px; max-width: 100px;"
            "height: 100px; min-height: 100px; max-height: 100px;"
            "border-radius: 55px;"
            "background-color: transparent;"
            "color: black;"
            "padding: 0;"
            "margin: 0;"
            "font-size: 12px;"
            "border: 5px solid transparent;}"

            "QToolButton:checked{"
            "border: 5px solid blue;}";
}

QPixmap MainWindow::maskPixMap(QPixmap pm){
    QPixmap scaledPixmap = pm.scaled(100, 100, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation).copy(0, 0, 100, 100);
    QImage out_img(100, 100, QImage::Format_ARGB32);

    out_img.fill(Qt::transparent);

    QBrush brush(scaledPixmap);
    QPainter painter(&out_img);
    painter.setBrush(brush);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Here if x and y of 'drawEllipse' are at 'n' and 'm', then width and height must be at
    // (100 -2*n) and (100 - 2*m) respectively in order to have a centered circular image
    painter.drawEllipse(3, 3, 94, 94);

    painter.end();

    QPixmap outPix = QPixmap::fromImage(out_img);
    return outPix;
}

// ===========================================
// Tray Icon and window methods
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

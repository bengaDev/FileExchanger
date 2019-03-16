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
    setWindowTitle("File Exchanger");
    if(icon != nullptr){
        setWindowIcon(QIcon(":/Icon_IMG/shareIMG.png"));
    }

    /*QPixmap pix = dataManager->localHost->getAvatar();
    QPixmap maskedPixMap = maskPixMap(pix);
    ui->toolButton->setIcon(QIcon(maskedPixMap));*/

    flowLayout_ScrollArea = new FlowLayout();

    //ui->BackBtn->setStyleSheet("QPushButton:hover{background-color: yellow}");

    // ============================

    ui->scrollArea->widget()->setLayout(flowLayout_ScrollArea);


    Host h1(true, "host1"); // = new Host(true, "host1");
    Host h2(true, "host2"); // = new Host(true, "host2");
    h1.createuniqueID();
    h2.createuniqueID();

    ///qDebug() << dataManager->getOnlineUsers().back().getName() << " : " << dataManager->getOnlineUsers().back().uniqueID;

    // Temp. connections just to simulate server updates
    connect(ui->ShareBtn, SIGNAL(clicked()), this, SLOT(onShareButton()));
    connect(ui->BackBtn, SIGNAL(clicked()), this, SLOT(DEBUG_addToDataManager()));

    // When a list in dataManager is updated, it triggers this SLOT, which will update the GUI
    connect(dataManager, SIGNAL(isUpdated()), this, SLOT(updateAvatarVisible()));

    // Connections for GUI receiver side
    connect(dataManager, SIGNAL(metadataStageEND(qint64, QString, QUuid, QString)), this, SLOT(messageBoxYES_NO(qint64, QString, QUuid, QString)));

    // Connect for Main Window activation
    connect(dataManager, SIGNAL(activateMainWindow(QString)), this, SLOT(on_activateMainWindow(QString)));
    ///dataManager->addOnlineUser(h1);
    ///dataManager->deleteOnlineUser(h1);

}

void MainWindow::messageBoxYES_NO(qint64 fileSize, QString fileName, QUuid id, QString senderName){

    QString filePath;
    double fileSizeKB = fileSize/1024.0;
    if(dataManager->getReceiveFilesAutom() == true){
        if(dataManager->getIFDefaultSavingPath() == true){
            emit dataManager->savingPath("./");
        }
        else{
            filePath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home");
            emit dataManager->savingPath(filePath);
        }
        //emit dataManager->messageBoxYes();
        addReceiverProgBar(id, senderName);
    }
    else {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Incoming file", "Do you want to accept file '"
                                      + fileName + "' of size " + QString::number(fileSizeKB) + " KB?", QMessageBox::Yes|QMessageBox::No);

        if(reply == QMessageBox::Yes){
            if(dataManager->getIFDefaultSavingPath() == true){
                emit dataManager->savingPath("./");
            }
            else{
                filePath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home");
                emit dataManager->savingPath(filePath);
            }
            //emit dataManager->messageBoxYes();
            addReceiverProgBar(id, senderName);
        }
        else if(reply == QMessageBox::No){
            emit dataManager->closeSocket();
        }
    }

}

void MainWindow::onShareButton(){

    std::list<Host> toSendUsers = dataManager->getToSendUsers();

    if(!toSendUsers.empty()){
        sendingWindow = new WindowProgressBar(this, dataManager);
        sendingWindow->setWindowTitle("Sending File");
        sendingWindow->setAttribute(Qt::WA_DeleteOnClose);

        sendingWindow->addProgressBars_SendTo(toSendUsers);
        //sendingWindow->addAllProgressBars(toSendUsers);

        sendingWindow->show();


        // SEND FILE
        emit dataManager->sendFile_SIGNAL();  //signal in datamanager that connects with the client
    }
}


void MainWindow::addReceiverProgBar(QUuid id, QString senderName){

    receiverWindow = new WindowProgressBar(this, dataManager);
    receiverWindow->setWindowTitle("Receiving file");
    receiverWindow->setAttribute(Qt::WA_DeleteOnClose);

    receiverWindow->addProgessBars_ReceivingFrom(id, senderName);

    if(!receiverWindow->isVisible()){
        receiverWindow->show();
    }

}

void MainWindow::DEBUG_addToDataManager(){
    Host temp = *(dataManager->localHost);
    dataManager->addOnlineUser(temp);
}
void MainWindow::DEBUG_clearDataManager(){
    dataManager->DEBUG_clearOnlineUsers();
    //ui->ListCounter->setText(QString::number(onlineUsersGUI->size()));
}

// This SLOT will be called when 'onlineUsers' in 'dataManager' is updated /
// when the SIGNAL 'isUpdated()' is called
void MainWindow::updateAvatarVisible(){

    std::list<Host> l = dataManager->getOnlineUsers();

    // First empty the current widgets from GUI Layout
    for(int i = 0; i < onlineUsersGUI->size(); i++){
        QWidget *lastItem = onlineUsersGUI->at(i);

        flowLayout_ScrollArea->removeWidget(lastItem); // removes widget from layout
        delete lastItem; // removes widget from memory
    }

    // Empty the list of pointers to those widgets
    onlineUsersGUI->clear();


    // Then fill it back updated
    for(std::list<Host>::iterator it=l.begin(); it != l.end(); it++){
        ContainerGUI *container = fromHost_to_Container(*it);
        onlineUsersGUI->push_back(container);

        flowLayout_ScrollArea->addWidget(onlineUsersGUI->last());
    }


}

ContainerGUI* MainWindow::fromHost_to_Container(Host h){

    QPixmap maskedPixmap = h.getAvatar();

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
    dataManager->addToSendUser(ID);
}

void MainWindow::deleteUuidToSend_SLOT(QUuid ID){
    dataManager->deleteToSendUser(ID);
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
    connect(quitAction, &QAction::triggered, this, &MainWindow::onQuitAction);
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

void MainWindow::on_activateMainWindow(QString fileName){
    if(!this->isVisible()){
        ui->sendFileLabel->setText("Sending file: " + fileName);
        this->show();
    }
}

void MainWindow::onBackButton(){
    this->hide();
}

void MainWindow::onQuitAction(){
    emit dataManager->quittingApplication();
}

void MainWindow::on_actionChange_Settings_triggered()
{
    settingsWindow = new SettingsWindow(this, dataManager);
    settingsWindow->setWindowTitle("Settings");
    settingsWindow->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

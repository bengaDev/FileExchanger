#include "windowprogressbar.h"
#include "ui_windowprogressbar.h"

WindowProgressBar::WindowProgressBar(QWidget *parent, Data_Manager *dm) :
    QDialog(parent),
    ui(new Ui::WindowProgressBar)
{
    ui->setupUi(this);
    this->dm = dm;


    vLayout_ScrollArea = new QVBoxLayout();
    ui->scrollArea->widget()->setLayout(vLayout_ScrollArea);

    //connect(this, SIGNAL(destroyed(QObject*)), dm, SLOT(DEBUG_trySlot(QObject*)));
    connect(dm, SIGNAL(setLabelProgBarWindow(QUuid, QString)), this, SLOT(onSetLabel(QUuid, QString)), Qt::QueuedConnection);


}

void WindowProgressBar::addProgressBars_SendTo(std::list<Host> toSendUsers){

    // It connects these signals only if in a "sender window"
    connect(dm, SIGNAL(setProgBarMaximum_SENDER(QUuid, qint64)), this, SLOT(onSetMaximumProgBar(QUuid, qint64)), Qt::QueuedConnection);
    connect(dm, SIGNAL(setProgBarValue_SENDER(QUuid, qint64)), this, SLOT(onSetValueProgBar(QUuid, qint64)), Qt::QueuedConnection);

    for(std::list<Host>::iterator host = toSendUsers.begin(); host != toSendUsers.end(); host++){

        QHBoxLayout *hLayout = new QHBoxLayout();
        QVBoxLayout *internalVLayout = new QVBoxLayout();


        QWidget *container = new QWidget(this);
        container->setLayout(hLayout);

        QWidget *progBar_and_label = new QWidget(container);
        progBar_and_label->setLayout(internalVLayout);

        QProgressBar *progressBar = new QProgressBar(container);
        QLabel *progBarLabel = new QLabel("Sending to " + host->getName() + "...", container);
        progressBar->setFixedWidth(400);

        internalVLayout->addWidget(progressBar);
        internalVLayout->addWidget(progBarLabel);
        internalVLayout->addStretch(1);

        QPushButton *cancelBtn = new QPushButton("Cancel", container);
        QUuid id = host->getUniqueID();       

        connect(cancelBtn, &QPushButton::clicked, [id, this](){
            qDebug() << "onInterruptSending: idStr = " +id.toString();
            emit dm->interruptSending(id);
        });

        hLayout->addWidget(progBar_and_label);
        hLayout->addWidget(cancelBtn);

        progressBarMap.insert(host->getUniqueID(), container);
    }

    displayList();


}

void WindowProgressBar::addProgessBars_ReceivingFrom(QUuid id, QString name){

    // It connects these signals only if in a "receiver window"
    connect(dm, SIGNAL(setProgBarMaximum_RECEIVER(QUuid, qint64)), this, SLOT(onSetMaximumProgBar(QUuid, qint64)));
    connect(dm, SIGNAL(setProgBarValue_RECEIVER(QUuid, qint64)), this, SLOT(onSetValueProgBar(QUuid, qint64)));

    QHBoxLayout *hLayout = new QHBoxLayout();
    QVBoxLayout *internalVLayout = new QVBoxLayout();


    QWidget *container = new QWidget(this);
    container->setLayout(hLayout);

    QWidget *progBar_and_label = new QWidget(container);
    progBar_and_label->setLayout(internalVLayout);

    QProgressBar *progressBar = new QProgressBar(container);
    QLabel *progBarLabel = new QLabel("Receiving from " + name + "...", container);
    progressBar->setFixedWidth(400);

    internalVLayout->addWidget(progressBar);
    internalVLayout->addWidget(progBarLabel);
    internalVLayout->addStretch(1);

    QPushButton *cancelBtn = new QPushButton("Cancel", container);

    connect(cancelBtn, &QPushButton::clicked, [id, this](){
        qDebug() << "onInterruptReceiving: idStr = " +id.toString();
        emit dm->interruptReceiving(id);
    });

    hLayout->addWidget(progBar_and_label);
    hLayout->addWidget(cancelBtn);

    progressBarMap.insert(id, container);


    displayList();

}

void WindowProgressBar::displayList(){

    QMapIterator<QUuid, QWidget*> it(progressBarMap);

    while(it.hasNext()){
        it.next();
        vLayout_ScrollArea->addWidget(it.value());
    }

    vLayout_ScrollArea->addStretch(1);

}

void WindowProgressBar::onSetMaximumProgBar(QUuid id, qint64 max){

    if(progressBarMap.find(id) != progressBarMap.end()){
        QWidget* container = progressBarMap.find(id).value();
        QProgressBar *progBar = container->findChild<QProgressBar*>();

        progBar->setMaximum(max);
    }else{
        qDebug() << "ERROR! NOT VALID ID in onSetMaximumProgBar";
    }
}

void WindowProgressBar::onSetValueProgBar(QUuid id, qint64 val){

    if(progressBarMap.find(id) != progressBarMap.end()){
        QWidget* container = progressBarMap.find(id).value();
        QProgressBar *progBar = container->findChild<QProgressBar*>();

        progBar->setValue(val);
    } else{
        qDebug() << "ERROR! NOT VALID ID in onSetValueProgBar";
    }
}

void WindowProgressBar::onSetLabel(QUuid id, QString stringLabel){
    if(progressBarMap.find(id) != progressBarMap.end()){
        QWidget* container = progressBarMap.find(id).value();
        QLabel* label = container->findChild<QLabel*>();

        label->setText(stringLabel);
    }
}

void WindowProgressBar::closeEvent(QCloseEvent *e){
    this->close();
    this->disconnect();
}

void WindowProgressBar::DEBUG_destroyedSlot(){
    qDebug() << "YEAH";
}

WindowProgressBar::~WindowProgressBar()
{
    delete ui;
}

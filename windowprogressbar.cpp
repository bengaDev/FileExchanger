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


    connect(dm, SIGNAL(setProgBarMaximum_SIGNAL(QUuid, qint64)), this, SLOT(onSetMaximumProgBar(QUuid, qint64)), Qt::QueuedConnection);
    connect(dm, SIGNAL(setProgBarValue_SIGNAL(QUuid, qint64)), this, SLOT(onSetValueProgBar(QUuid, qint64)), Qt::QueuedConnection);

}

void WindowProgressBar::addAllProgressBars(std::list<Host> toSendUsers){

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

        hLayout->addWidget(progBar_and_label);

        progressBarMap.insert(host->getUniqueID(), container);
    }

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
    QWidget* container = progressBarMap.find(id).value();
    QProgressBar *progBar = container->findChild<QProgressBar*>();

    progBar->setMaximum(max);

}

void WindowProgressBar::onSetValueProgBar(QUuid id, qint64 val){
    QWidget* container = progressBarMap.find(id).value();
    QProgressBar *progBar = container->findChild<QProgressBar*>();

    progBar->setValue(val);
}

void WindowProgressBar::DEBUG_destroyedSlot(){
    qDebug() << "YEAH";
}

WindowProgressBar::~WindowProgressBar()
{
    delete ui;
}

#include "containergui.h"

ContainerGUI::ContainerGUI(QWidget *parent, QPixmap avatarPixmap) : QWidget(parent)
{
    vLayout = new QVBoxLayout();
    this->setLayout(vLayout);

    avatarBtn = new QToolButton();

    avatarBtn->setCheckable(true);
    avatarBtn->setAutoRaise(true);
    avatarBtn->setIconSize(QSize(100, 100));
    avatarBtn->setIcon(QIcon(avatarPixmap));
    avatarBtn->setStyleSheet(avatarStyleSheet());

    vLayout->addWidget(avatarBtn);

    connect(avatarBtn, SIGNAL(toggled(bool)), this, SLOT(reactToAvatarPressed(bool)));

    avatarName = new QLabel(avatarBtn);
    vLayout->addWidget(avatarName);
}

void ContainerGUI::setUniqueID(QUuid uID){
    this->uniqueId = uID;
}

void ContainerGUI::setAvatarPixmap(QPixmap pm){
    this->scaledPixmap = pm;
}

void ContainerGUI::setAvatarLabel(QString name){
    avatarName->setText(name);
    avatarName->setAlignment(Qt::AlignCenter);
}

QString ContainerGUI::avatarStyleSheet(){
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

void ContainerGUI::reactToAvatarPressed(bool state){
    if(state == true){
        emit addUuidToSend_SIGNAL(uniqueId);
    } else if(state == false) {
        emit deleteUuidToSend_SIGNAL(uniqueId);
    }
}

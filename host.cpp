#include "host.h"

#include <QDebug>

Host::Host(bool visible, QString name)
{
    this->visible = visible;
    this->name = name;
    avatar = new QPixmap(":/Icon_IMG/avatar_2.png");

}

bool Host::isVisible()
{
    return this->visible;
}

QPixmap Host::getAvatar(){
    return *avatar;
}

QString Host::getName(){
    return name;
}

void Host::createuniqueID(){
    uniqueID = QUuid::createUuid();

    qDebug() << this << " " << this->name << " : " << uniqueID;
}

QUuid Host::getUniqueID(){
    return this->uniqueID;
}

bool Host::operator==(Host h){
    return this->uniqueID == h.uniqueID ? true : false;
}

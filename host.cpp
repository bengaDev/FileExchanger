#include "host.h"

#include <QDebug>

Host::Host(bool visible, QString name)
{
    this->visible = visible;
    this->name = name;
    lastSeen = time(nullptr);
    //avatar = new QPixmap(maskPixmap(QPixmap("C:/Users/Benjamin/Desktop/Poli/Prog_SISTEMA/PROGETTO/PDS_fileExchanger/Icon_IMG/avatar_4.png")));
    //avatar = new QPixmap(maskPixmap(QPixmap("./../PDS_fileExchanger/Icon_IMG/avatar_4.png")));
    //QString a(":\\Icon_IMG\\avatar_2.png");
    //a.replace('\\', '/');
    //avatar = new QPixmap(maskPixmap(QPixmap(a)));
    avatar = new QPixmap(maskPixmap(QPixmap(":/Icon_IMG/avatar_2.png")));

}

void Host::setVisibility(bool visibility){
    this->visible = visibility;
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

void Host::setName(QString name){
    this->name = name;
}

void Host::createuniqueID(){
    uniqueID = QUuid::createUuid();

    qDebug() << this << " " << this->name << " : " << uniqueID;
}

QUuid Host::getUniqueID(){

    return this->uniqueID;
}

void Host::setUniqueID(QUuid uniqueID){
    this->uniqueID = uniqueID;
}

QString Host::getVisibilityStatus(){
    return (visible==true) ? "VISIBLE" : "NOTVISIBLE";  //ENUM
}

void Host::setIP(QHostAddress ip){
    this->ipAddr = ip;
}

QHostAddress Host::getIP(){
    return ipAddr;
}

void Host::setLastSeen(time_t time){
    lastSeen = time;
}

time_t Host::getLastSeen(){
    return lastSeen;
}

QPixmap Host::maskPixmap(QPixmap pm){
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

void Host::setAvatar(QPixmap avatar){
    (*this->avatar) = avatar;
}

bool Host::operator==(Host h){
    return this->uniqueID == h.uniqueID ? true : false;
}

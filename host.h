#ifndef HOST_H
#define HOST_H
#include <string>
#include <ctime>
#include <QPixmap>
#include <QUuid>
#include <QBrush>
#include <QPainter>
#include <QHostAddress>

class Host
{
public:
    Host(bool visible = true, QString name = "none");
    Host(const Host& h);
    bool isVisible();
    QPixmap getAvatar();
    QString getName();
    void setName(QString);
    void createuniqueID();
    QUuid getUniqueID();
    QString getVisibilityStatus();
    void setVisibility(bool);
    void setAvatar(QPixmap);
    void setUniqueID(QUuid uniqueID);
    bool operator==(Host h);
    void setIP(QHostAddress);
    QHostAddress getIP();
    void setLastSeen(time_t time);
    time_t getLastSeen();

    QUuid uniqueID; // TO PUT IN PRIVATE IN FINAL VERSION ----

private:
    bool visible;
    QString name;
    QPixmap* avatar;
    QHostAddress ipAddr;
    time_t lastSeen;
    QPixmap maskPixmap(QPixmap);


    //address ip;
    //picture avatar;
};

#endif // HOST_H

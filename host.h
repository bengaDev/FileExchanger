#ifndef HOST_H
#define HOST_H
#include <string>
#include <QPixmap>
#include <QUuid>
#include <QBrush>
#include <QPainter>

class Host
{
public:
    Host(bool visible = true, QString name = "none");
    bool isVisible();
    QPixmap getAvatar();
    QString getName();
    void createuniqueID();
    QUuid getUniqueID();
    bool operator==(Host h);

    QUuid uniqueID; // TO PUT IN PRIVATE IN FINAL VERSION ----

private:
    bool visible;
    QString name;
    QPixmap* avatar;

    QPixmap maskPixmap(QPixmap);


    //address ip;
    //picture avatar;
};

#endif // HOST_H

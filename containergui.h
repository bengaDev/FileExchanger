#ifndef CONTAINERGUI_H
#define CONTAINERGUI_H

#include <QWidget>
#include <QUuid>
#include <QToolButton>
#include <QVBoxLayout>
#include <QLabel>

class ContainerGUI : public QWidget
{
    Q_OBJECT
public:
    explicit ContainerGUI(QWidget *parent = nullptr, QPixmap avatarPixmap = QPixmap(":/Icon_IMG/avatar_2.png"));
    void setUniqueID(QUuid uID);
    void setAvatarPixmap(QPixmap pm);
    void setAvatarLabel(QString name);
    QString avatarStyleSheet();
    QUuid uniqueId; // TO PUT IN PRIVATE IN FINAL VERSION

private:
    QToolButton* avatarBtn;
    QVBoxLayout* vLayout;
    QPixmap scaledPixmap;
    QLabel* avatarName;


signals:
    void deleteUuidToSend_SIGNAL(QUuid ID);
    void addUuidToSend_SIGNAL(QUuid ID);

public slots:
    void reactToAvatarPressed(bool state);
};

#endif // CONTAINERGUI_H

#ifndef WINDOWPROGRESSBAR_H
#define WINDOWPROGRESSBAR_H

#include <QDialog>
#include <QBoxLayout>
#include <QPushButton>
#include <QProgressBar>
#include <QMap>
#include <QSignalMapper>

#include "data_manager.h"
#include "flowlayout.h"

namespace Ui {
class WindowProgressBar;
}

class WindowProgressBar : public QDialog
{
    Q_OBJECT

public:
    explicit WindowProgressBar(QWidget *parent = nullptr, Data_Manager* dm = nullptr);
    ~WindowProgressBar();

    void addProgressBars_SendTo(std::list<Host> toSendUsers);
    void addProgessBars_ReceivingFrom(QUuid, QString);

    void closeEvent(QCloseEvent *e) override;

private slots:
    void onSetMaximumProgBar(QUuid, qint64);
    void onSetValueProgBar(QUuid, qint64);
    void onSetLabel(QUuid, QString);


private:
    Ui::WindowProgressBar *ui;
    Data_Manager *dm;
    QMap<QUuid, QWidget*> progressBarMap;
    QVBoxLayout *vLayout_ScrollArea;

    void displayList();

};

#endif // WINDOWPROGRESSBAR_H

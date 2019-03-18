#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QStandardPaths>
#include "data_manager.h"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr, Data_Manager* dm = nullptr);
    ~SettingsWindow();

private slots:
    void on_visibilityCheckBox_stateChanged(int arg1);

    void on_lineEdit_UserName_editingFinished();

    void on_automFileReceiveCheckBox_stateChanged(int arg1);

    void on_defaultSavePathCheckBox_stateChanged(int arg1);

    void on_okPushBtn_clicked();

    void on_cancelPushBtn_clicked();

private:
    Ui::SettingsWindow *ui;
    Data_Manager *dm;

    bool isVisible;
    QString userName;
    bool receivesFileAutom;
    bool isDefaultPath;
};

#endif // SETTINGSWINDOW_H

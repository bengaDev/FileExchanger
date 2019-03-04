#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
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

private:
    Ui::SettingsWindow *ui;
    Data_Manager *dm;
};

#endif // SETTINGSWINDOW_H

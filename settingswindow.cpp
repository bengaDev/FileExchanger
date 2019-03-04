#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent, Data_Manager *dm) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    this->dm = dm;
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::on_visibilityCheckBox_stateChanged(int state)
{
    if(state == Qt::Unchecked){
        dm->setLocalHostVisibility(false);
    }
    else if(state == Qt::Checked){
        dm->setLocalHostVisibility(true);
    }
}

void SettingsWindow::on_lineEdit_UserName_editingFinished()
{
    QString newLocalHostName = ui->lineEdit_UserName->text();
    dm->setLocalHostName(newLocalHostName);
}

void SettingsWindow::on_automFileReceiveCheckBox_stateChanged(int state)
{
    if(state == Qt::Unchecked){
        dm->setReceiveFilesAutom(false);
    }
    else if(state == Qt::Checked){
        dm->setReceiveFilesAutom(true);
    }
}

void SettingsWindow::on_defaultSavePathCheckBox_stateChanged(int state)
{
    if(state == Qt::Unchecked){
        dm->setIFDefaultSavingPath(false);
    }
    else if(state == Qt::Checked){
        dm->setIFDefaultSavingPath(true);
    }
}

#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent, Data_Manager *dm) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    this->dm = dm;
    isVisible = dm->localHost->isVisible();
    userName = dm->localHost->getName();
    receivesFileAutom = dm->getReceiveFilesAutom();
    isDefaultPath = dm->getIFDefaultSavingPath();

    ui->visibilityCheckBox->setCheckState( (isVisible == true) ? Qt::Checked : Qt::Unchecked );
    ui->lineEdit_UserName->setText(userName);
    ui->automFileReceiveCheckBox->setCheckState( (receivesFileAutom == true) ? Qt::Checked : Qt::Unchecked );
    ui->defaultSavePathCheckBox->setCheckState( (isDefaultPath == true) ? Qt::Checked : Qt::Unchecked );
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::on_visibilityCheckBox_stateChanged(int state)
{
    if(state == Qt::Unchecked){
        isVisible = false;
    }
    else if(state == Qt::Checked){
        isVisible = true;
    }
}

void SettingsWindow::on_lineEdit_UserName_editingFinished()
{
    userName = ui->lineEdit_UserName->text();

}

void SettingsWindow::on_automFileReceiveCheckBox_stateChanged(int state)
{
    if(state == Qt::Unchecked){
        receivesFileAutom = false;
    }
    else if(state == Qt::Checked){
        receivesFileAutom = true;
    }
}

void SettingsWindow::on_defaultSavePathCheckBox_stateChanged(int state)
{
    if(state == Qt::Unchecked){
        isDefaultPath = false;
    }
    else if(state == Qt::Checked){
        isDefaultPath = true;
    }
}

void SettingsWindow::on_okPushBtn_clicked()
{
    dm->setLocalHostVisibility(isVisible);
     dm->setLocalHostName(userName);
     dm->setReceiveFilesAutom(receivesFileAutom);
     dm->setIFDefaultSavingPath(isDefaultPath);
     this->close();
}

void SettingsWindow::on_cancelPushBtn_clicked()
{
    this->close();
}

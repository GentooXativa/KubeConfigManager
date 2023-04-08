#include <QDir>
#include "settingsform.h"
#include "ui_settingsform.h"

SettingsForm::SettingsForm(QSettings *appSettings, QWidget *parent) : QDialog(parent),
                                                                      ui(new Ui::SettingsForm)
{
    ui->setupUi(this);
    this->appSettings = appSettings;
    this->updateUI();
}

SettingsForm::~SettingsForm()
{
    delete ui;
}

void SettingsForm::on_buttonBox_accepted()
{
    appSettings->setValue("paths/working_directory", ui->lineEditWorkingDirectory->text());
    appSettings->setValue("backup/configuration", ui->checkBoxBackupConfig->isChecked());

    if (!ui->lineEditDisabledFolderPath->text().isEmpty())
    {
        appSettings->setValue("paths/disabled_directory", ui->lineEditDisabledFolderPath->text());
    }

    emit settingsUpdated();
    this->close();
}

void SettingsForm::on_buttonBox_rejected()
{
    this->close();
}

void SettingsForm::on_toolButtonWorkingDirectory_triggered(QAction *arg1)
{
}

void SettingsForm::on_toolButtonDisabledFolderPath_triggered(QAction *arg1)
{
}

void SettingsForm::updateUI()
{
    if (appSettings->contains("paths/working_directory"))
    {
        ui->lineEditWorkingDirectory->setText(appSettings->value("paths/working_directory").toString());
        if (!appSettings->contains("paths/disabled_directory"))
        {
            ui->lineEditDisabledFolderPath->setText(QString("%1/disabled.conf.d").arg(appSettings->value("paths/working_directory").toString()));
        }
    }
    else
    {
        QString initialDirectory = QDir::homePath();
        initialDirectory.append("/.kube");

        QDir kubedir = QDir(initialDirectory);

        if (!kubedir.exists())
        {
            initialDirectory = QDir::homePath();
        }

        ui->lineEditWorkingDirectory->setText(initialDirectory);
    }

    if (appSettings->contains("paths/disabled_directory"))
    {
        ui->lineEditDisabledFolderPath->setText(appSettings->value("paths/disabled_directory").toString());
    }
}

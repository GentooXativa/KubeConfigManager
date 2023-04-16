#include <QDir>
#include <QDebug>
#include <QProcessEnvironment>
#include <QMessageBox>
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
    if (ui->lineEditWorkingDirectory->text().isEmpty())
    {
        QMessageBox::critical(this, tr("Please, configure this application"), tr("You need to provide at least a working directory to continue, this directory will hold all the configuration files."));
        return;
    }
    appSettings->setValue("paths/working_directory", ui->lineEditWorkingDirectory->text());
    appSettings->setValue("backup/configuration", ui->checkBoxBackupConfig->isChecked());
    appSettings->setValue("ui/start_minimized", ui->checkBoxStartMinimized->isChecked());
    appSettings->setValue("ui/start_hidden", ui->checkBoxStartHidden->isChecked());
    appSettings->setValue("ui/show_filepanel", ui->checkBoxShowFilePanel->isChecked());

    if (!ui->lineEditDisabledFolderPath->text().isEmpty())
    {
        appSettings->setValue("paths/disabled_directory", ui->lineEditDisabledFolderPath->text());
    }

    emit settingsUpdated();
    this->close();
}

void SettingsForm::on_buttonBox_rejected()
{
    if (appSettings->allKeys().length() == 0)
    {
        QMessageBox::critical(this, tr("Please, configure this application"), tr("You need to provide at least a working directory to continue, this directory will hold all the configuration files."));
    }
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
            QMessageBox::warning(this, tr("Default directory not found"), tr("Default directory for kubectl not found, please provide a working directory."));
            return;
        }

        ui->lineEditWorkingDirectory->setText(initialDirectory);
    }

    if (appSettings->contains("paths/disabled_directory"))
    {
        ui->lineEditDisabledFolderPath->setText(appSettings->value("paths/disabled_directory").toString());
    }

    if (appSettings->contains("ui/start_minimized"))
        ui->checkBoxStartMinimized->setChecked(appSettings->value("ui/start_minimized").toBool());

    if (appSettings->contains("ui/start_hidden"))
        ui->checkBoxStartHidden->setChecked(appSettings->value("ui/start_hidden").toBool());
}

void SettingsForm::on_pushButtonKubeCtlCheck_clicked()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString pathString = env.value("PATH");
    QStringList paths = pathString.split(":");

    for (QStringList::iterator it = paths.begin(); it != paths.end(); it++)
    {
        QString path = *it;
        QString filepath(QString("%1/kubectl").arg(path));
        QFile executable(filepath);
        if (executable.exists())
        {
            QProcess testRun;
            testRun.setProgram(filepath);
            QStringList args;
            args << "version";
            testRun.setArguments(args);
            qDebug() << "FIle found, trying to execute:" << filepath;

            testRun.start();

            if (!testRun.waitForFinished())
            {
                qDebug() << "kubectl failed:" << testRun.errorString();
                // @todo show a error message about permissions
            }
            else
            {
                qDebug() << "Success: " << testRun.readAll();
                ui->lineEditKubeCtlPath->setText(filepath);
            }
        }
    }
}

void SettingsForm::on_checkBoxStartHidden_toggled(bool checked)
{
    if (checked)
        this->ui->checkBoxStartMinimized->setChecked(!checked);
}

void SettingsForm::on_checkBoxStartMinimized_toggled(bool checked)
{
    if (checked)
        this->ui->checkBoxStartHidden->setChecked(!checked);
}

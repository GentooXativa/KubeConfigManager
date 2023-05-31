#include <QDir>
#include <QDebug>
#include <QProcessEnvironment>
#include <QMessageBox>
#include <QFileDialog>
#include "settingsform.h"
#include "ui_settingsform.h"

SettingsForm::SettingsForm(QSettings *appSettings, QWidget *parent) : QDialog(parent),
                                                                      ui(new Ui::SettingsForm)
{
    ui->setupUi(this);
    this->appSettings = appSettings;
    this->updateUI();
    ui->spinBoxConfigBackupHistorySize->setMinimum(1);
    ui->spinBoxConfigBackupHistorySize->setMaximum(99);
#ifdef QT_DEBUG
    // if there is no settings and we are under DEBUG build, we add a save
    // on each change
    this->ui->groupBoxDeveloper->setVisible(true);
    if (appSettings->allKeys().length() == 0)
    {
        this->ui->checkBoxDevSaveAllChanges->setChecked(true);
    }
#else
    this->ui->groupBoxDeveloper->setVisible(false);
#endif

    connect(ui->checkBoxBackupConfig, &QCheckBox::stateChanged, this, [=]()
            { 
                ui->lineConfigBackupDirectory->setEnabled(ui->checkBoxBackupConfig->isChecked()); 
                ui->spinBoxConfigBackupHistorySize->setEnabled(ui->checkBoxBackupConfig->isChecked()); 
                ui->toolButtonConfigBackupDirectory->setEnabled(ui->checkBoxBackupConfig->isChecked()); });

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsForm::onSaveConfig);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &SettingsForm::onReject);

    connect(ui->checkBoxStartHidden, &QCheckBox::stateChanged, this, [=]()
            { 
                if(ui->checkBoxStartHidden->isChecked())
                    ui->checkBoxStartMinimized->setChecked(false); });

    connect(ui->checkBoxStartMinimized, &QCheckBox::stateChanged, this, [=]()
            { 
                if(ui->checkBoxStartMinimized->isChecked())
                    ui->checkBoxStartHidden->setChecked(false); });

    connect(ui->toolButtonConfigBackupDirectory, &QToolButton::clicked, this, [=]()
            { this->onSearchFolder(ui->lineConfigBackupDirectory); });
    connect(ui->toolButtonDisabledFolderPath, &QToolButton::clicked, this, [=]()
            { this->onSearchFolder(ui->lineEditDisabledFolderPath); });
    connect(ui->toolButtonWorkingDirectory, &QToolButton::clicked, this, [=]()
            { this->onSearchFolder(ui->lineEditWorkingDirectory); });

    connect(ui->pushButtonKubeCtlCheck, &QPushButton::clicked, this, &SettingsForm::onTryToFindKubectl);
}

SettingsForm::~SettingsForm()
{
    delete ui;
}

void SettingsForm::onSaveConfig()
{
    if (ui->lineEditWorkingDirectory->text().isEmpty())
    {
        QMessageBox::critical(this, tr("Please, configure this application"), tr("You need to provide at least a working directory to continue, this directory will hold all the configuration files."));
        return;
    }

    if (ui->checkBoxBackupConfig->isChecked() && ui->lineConfigBackupDirectory->text().isEmpty())
    {
        QMessageBox::critical(this, tr("Please provide a backup directory"), tr("You need to provide a backup folder to place all the backups or disable this feature."));
        this->ui->lineConfigBackupDirectory->setFocus();
        return;
    }

    appSettings->setValue("paths/working_directory", ui->lineEditWorkingDirectory->text());
    appSettings->setValue("backup/configuration", ui->checkBoxBackupConfig->isChecked());
    appSettings->setValue("ui/start_minimized", ui->checkBoxStartMinimized->isChecked());
    appSettings->setValue("ui/start_hidden", ui->checkBoxStartHidden->isChecked());
    appSettings->setValue("ui/show_filepanel", ui->checkBoxShowFilePanel->isChecked());

#ifdef QT_DEBUG
    appSettings->setValue("dev/save_all_changes", ui->checkBoxDevSaveAllChanges->isChecked());
#endif

    if (ui->checkBoxBackupConfig->isChecked())
    {
        appSettings->setValue("paths/backup", ui->lineConfigBackupDirectory->text());
        appSettings->setValue("backup/history_size", ui->spinBoxConfigBackupHistorySize->value());
    }

    if (!ui->lineEditDisabledFolderPath->text().isEmpty())
    {
        appSettings->setValue("paths/disabled_directory", ui->lineEditDisabledFolderPath->text());
    }

    if (!ui->lineEditKubeCtlPath->text().isEmpty())
    {
        appSettings->setValue("paths/kubectl", ui->lineEditKubeCtlPath->text());
    }

    appSettings->sync();
    emit settingsUpdated();
    this->close();
}

void SettingsForm::onReject()
{
    if (appSettings->allKeys().length() == 0)
    {
        QMessageBox::critical(this, tr("Please, configure this application"), tr("You need to provide at least a working directory to continue, this directory will hold all the configuration files."));
    }
    this->close();
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

    if (appSettings->contains("paths/kubectl"))
        ui->lineEditKubeCtlPath->setText(appSettings->value("paths/kubectl").toString());

    if (appSettings->contains("dev/save_all_changes"))
        ui->checkBoxDevSaveAllChanges->setChecked(appSettings->value("dev/save_all_changes").toBool());

    if (appSettings->contains("backup/configuration"))
    {
        ui->checkBoxBackupConfig->setChecked(appSettings->value("backup/configuration").toBool());

        ui->lineConfigBackupDirectory->setEnabled(ui->checkBoxBackupConfig->isChecked());
        ui->toolButtonConfigBackupDirectory->setEnabled(ui->checkBoxBackupConfig->isChecked());

        if (appSettings->contains("backup/history_size"))
            ui->spinBoxConfigBackupHistorySize->setValue(appSettings->value("backup/history_size").toInt());
        else
            ui->spinBoxConfigBackupHistorySize->setValue(5);

        if (appSettings->contains("paths/backup"))
        {
            ui->lineConfigBackupDirectory->setText(appSettings->value("paths/backup").toString());
        }
        else
        {
            if (!ui->lineEditWorkingDirectory->text().isEmpty())
            {
                ui->lineConfigBackupDirectory->setText(QString("%1/backup.conf.d").arg(ui->lineEditWorkingDirectory->text()));
            }
        }
    }
}

void SettingsForm::onTryToFindKubectl()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString pathString = env.value("PATH");
    QStringList paths = pathString.split(":");

    for (QStringList::iterator it = paths.begin(); it != paths.end(); ++it)
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
                QMessageBox::critical(this, tr("Error triggering kubectl"), QString(tr("An error happened trying to execute kubectl:\n\n%1")).arg(testRun.errorString()));
                return;
            }
            else
            {
                qDebug() << "Success: " << testRun.readAll();
                ui->lineEditKubeCtlPath->setText(filepath);
                return;
            }
        }
    }

    QMessageBox::information(this, tr("kubectl not found"), tr("Cannot found \"kubectl\" command on your PATH settings, please find the location to be able to use it."));
}

void SettingsForm::onSearchFolder(QLineEdit *target)
{
    qDebug() << "Triggered on:" << target->objectName();
    QString initialDirectory;

    if (!target->text().isEmpty())
        initialDirectory = target->text();
    else if (!ui->lineEditWorkingDirectory->text().isEmpty())
        initialDirectory = ui->lineEditWorkingDirectory->text();
    else
        initialDirectory = QDir::home().absolutePath();

    qDebug() << "Showing initial directory:" << initialDirectory;

    QFileDialog *directorySelector = new QFileDialog(this, tr("Please select your directory"), initialDirectory);
    directorySelector->setFileMode(QFileDialog::Directory);
    directorySelector->setViewMode(QFileDialog::List);
    directorySelector->setOption(QFileDialog::ShowDirsOnly, true);

    if (directorySelector->exec())
    {
        QStringList selected = directorySelector->selectedFiles();
        target->setText(selected.at(0));
    }
}
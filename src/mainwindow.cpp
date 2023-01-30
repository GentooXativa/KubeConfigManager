#include "KubeConfManager.h"

#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QStringListModel>

#include "kubeparser.h"
#include "settingsform.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    uiHasBeenInitialized = false;
    appSettings = new QSettings(QSettings::UserScope, "GentooXativa", "KubeConfManager", this);

    ui->setupUi(this);

    contextsModel = new QStringListModel(this);
    ui->listViewContexts->setModel(contextsModel);

    this->loadSettings();

    if (!workingDirectory.isEmpty())
    {
        ui->lineEditWorkingDirectory->setText(workingDirectory);
    }

    this->clearView();

    uiHasBeenInitialized = true;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    this->close();
}

void MainWindow::on_toolButtonWorkingDirectory_clicked()
{
    QString initialDirectory = QDir::homePath();
    initialDirectory.append("/.kube");

    QDir kubedir = QDir(initialDirectory);

    if (!kubedir.exists())
    {
        initialDirectory = QDir::homePath();
    }

    QFileDialog *directorySelector = new QFileDialog(this, "Please select your .kube directory", initialDirectory);
    directorySelector->setFileMode(QFileDialog::Directory);
    directorySelector->setViewMode(QFileDialog::List);
    directorySelector->setOption(QFileDialog::ShowDirsOnly, true);

    if (directorySelector->exec())
    {
        QStringList selected = directorySelector->selectedFiles();
        setWorkingDirectory(selected.at(0), true);
    }

    return;
}

void MainWindow::on_actionReload_triggered()
{
    if (workingDirectory.isEmpty())
    {
        QMessageBox::critical(this, "Error", "You need to select a working directory first!");
        return;
    }

    contextModelUpdated(QStringList());

    QDir directory(workingDirectory);
    QStringListModel *fileModel = new QStringListModel(directory.entryList(QDir::Files), this);
    ui->listViewFiles->setModel(fileModel);
}

void MainWindow::loadSettings()
{
    QStringList keys = appSettings->allKeys();
    qDebug() << "Keys found in config:" << keys.length();

    if (appSettings->contains("paths/working_directory"))
    {
        setWorkingDirectory(appSettings->value("paths/working_directory").toString(), false);
    }

    QFileInfo settingsFile(appSettings->fileName());
    QDir settingsDirectory(settingsFile.absoluteDir());

    qDebug() << "Configuration stored at:" << settingsDirectory.path();
}

void MainWindow::setWorkingDirectory(QString path, bool updateSettings)
{
    workingDirectory = path;
    qDebug() << "Working directory set to:" << path;

    if (uiHasBeenInitialized)
    {
        ui->lineEditWorkingDirectory->setText(path);
    }

    if (updateSettings)
    {
        appSettings->setValue("paths/working_directory", path);
        qDebug() << "Working directory settings saved";
    }
}

void MainWindow::on_listViewFiles_activated(const QModelIndex &index)
{
    qDebug() << "Activated on:" << index.data().toString();
    QString path = workingDirectory;
    path.append("/");
    path.append(index.data().toString());

    KubeParser parser(path, this);

    connect(&parser, &KubeParser::errorLoadingFile, this, &MainWindow::errorLoadingFileParser);

    connect(&parser, &KubeParser::contextsLoaded, this, &MainWindow::contextModelUpdated);

    ui->textEditContextInformation->setMarkdown("Cluster test\n* First\n* Second");

    parser.load();
}

void MainWindow::contextModelUpdated(const QStringList contexts)
{
    contextsModel->setStringList(contexts);
}

void MainWindow::errorLoadingFileParser(const QString message)
{
    QMessageBox::critical(this, "Error parsing file", message);
}

void MainWindow::on_actionSettings_triggered()
{
    SettingsForm *settings = new SettingsForm(appSettings, this);
    settings->setModal(true);
    settings->show();

    connect(settings, &SettingsForm::settingsUpdated, this, &MainWindow::clearView);
}

void MainWindow::clearView()
{
    this->loadSettings();
    this->checkResourcesAndDirectories();

    contextsModel->setStringList(QStringList());

    ui->textEditContextInformation->setMarkdown(
                QString("# Welcome to %1 v%2\n\n---\n* This section is a work in progress...\n\n\n ![Test](https://www.one-beyond.com/wpcms/wp-content/themes/dcsl/assets/images/logo-animated.gif)")
                .arg(APP_NAME)
                .arg(APP_VERSION)
    );
}

bool MainWindow::checkResourcesAndDirectories(){
    return true;
}

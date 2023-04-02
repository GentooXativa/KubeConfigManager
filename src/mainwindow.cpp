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
    this->clearView();

    if (!workingDirectory.isEmpty())
    {
        ui->lineEditWorkingDirectory->setText(workingDirectory);
        this->on_actionReload_triggered();
    }

    uiHasBeenInitialized = true;
    this->createTrayIcon();

    connect(this, &MainWindow::contextHasBeenSelected, this, &MainWindow::onContextSelected);
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
    connect(&parser, &KubeParser::contextListLoaded, this, &MainWindow::contextListUpdated);

    parser.load();
//    systemTrayIcon->showMessage("Test", "test 2",QSystemTrayIcon::Information);
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
                .arg(APP_NAME, APP_VERSION)
    );
}

bool MainWindow::checkResourcesAndDirectories(){
    return true;
}

void MainWindow::createTrayIcon(){
    systemTrayMenu = new QMenu(this);
    systemTrayMenu->addAction(ui->actionSwitchContext);
    systemTrayMenu->addSeparator();
    systemTrayMenu->addAction(ui->actionQuit);

    appIcon = new QIcon(":/logo/assets/logo.png" );

    systemTrayIcon = new QSystemTrayIcon( *appIcon, this);
    systemTrayIcon->setVisible(true);
    systemTrayIcon->setContextMenu(systemTrayMenu);
}

void MainWindow::on_actionSwitchContext_triggered()
{
    QDialog mine(this);
}

void MainWindow::reloadDefaultConfiguration(){

}

void MainWindow::on_listViewContexts_activated(const QModelIndex &index)
{
    qDebug() << "Context activated:" << index.data().toString();
    qDebug() << "Contexts loaded:" << this->contexts->size();
    for (auto i = this->contexts->begin(), end = this->contexts->end(); i != end; ++i){
         KubeContext current = *i;
         if( current.name == index.data().toString()){
             this->selectedContext = current;
             emit contextHasBeenSelected();
             return;
         }
    }
}


void MainWindow::contextListUpdated(KubeContextList *contexts) {
    this->contexts = contexts;
}

void MainWindow::onContextSelected(){
    qDebug() << "New context selected:" << this->selectedContext.name;
    this->updateContextInformationText();
}

void MainWindow::updateContextInformationText(){
    QString text(QString("Cluster %1\n    User: %2\n").arg(
        this->selectedContext.name,
        this->selectedContext.name
    ));
    ui->textEditContextInformation->setMarkdown(text);
}

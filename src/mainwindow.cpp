#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStringListModel>
#include "kubeparser.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    uiHasBeenInitialized = false;
    appSettings = new QSettings(QSettings::UserScope, "GentooXativa", "KubeConfManager", this );
    loadSettings();
    ui->setupUi(this);

    if( !workingDirectory.isEmpty()){
        ui->lineEditWorkingDirectory->setText(workingDirectory);
    }

    clustersModel = new QStringListModel(this);
    ui->listViewClusters->setModel(clustersModel);
    
    contextsModel = new QStringListModel(this);
    ui->listViewContexts->setModel(contextsModel);
    
    usersModel = new QStringListModel(this);
    ui->listViewUsers->setModel(usersModel);
    
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

    if( !kubedir.exists()){
        initialDirectory = QDir::homePath();
    }

    QFileDialog *directorySelector = new QFileDialog(this, "Please select your .kube directory", initialDirectory);
    directorySelector->setFileMode(QFileDialog::Directory);
    directorySelector->setViewMode(QFileDialog::List);
    directorySelector->setOption(QFileDialog::ShowDirsOnly, true);

    if( directorySelector->exec() ){
        QStringList selected = directorySelector->selectedFiles();
        setWorkingDirectory(selected.at(0), true );        
    }
    
    return;
}

void MainWindow::on_actionReload_triggered()
{
    if( workingDirectory.isEmpty() ){
        QMessageBox::critical(this, "Error", "You need to select a working directory first!");
        return;
    }

    QDir directory(workingDirectory);
    QStringListModel *fileModel = new QStringListModel(directory.entryList(QDir::Files), this );
    ui->listViewFiles->setModel(fileModel);
}

void MainWindow::loadSettings(){
    QStringList keys = appSettings->allKeys();
    qDebug() << "Keys found in config:" << keys.length();

    if( appSettings->contains("working_directory")){
        setWorkingDirectory( appSettings->value("working_directory").toString(), false );
    }
}

void MainWindow::setWorkingDirectory(QString path, bool updateSettings ){
    workingDirectory = path;
    qDebug() << "Working directory set to:" << path;

    if( uiHasBeenInitialized ){
        ui->lineEditWorkingDirectory->setText(path);
    }

    if( updateSettings ){
        appSettings->setValue("working_directory", path );
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

    connect(&parser, &KubeParser::clustersLoaded, this, &MainWindow::clusterModelUpdated);
    connect(&parser, &KubeParser::contextsLoaded, this, &MainWindow::contextModelUpdated);
    connect(&parser, &KubeParser::usersLoaded, this, &MainWindow::userModelUpdated);

    parser.load();
}

void MainWindow::clusterModelUpdated(const QStringList clusters ){
    clustersModel->setStringList(clusters);
}

void MainWindow::contextModelUpdated(const QStringList contexts ){
    contextsModel->setStringList(contexts);
}

void MainWindow::userModelUpdated(const QStringList users ){
    usersModel->setStringList(users);
}

void MainWindow::errorLoadingFileParser(const QString message){
    QMessageBox::critical(this, "Error parsing file", message );
}
#include "KubeConfManager.h"

#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QStringListModel>

#include "kubeparser.h"
#include "settingsform.h"
#include "contexteditor.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    uiHasBeenInitialized = false;
    setAcceptDrops(true);
    qApp->installEventFilter(this);
    appSettings = new QSettings(QSettings::UserScope, "GentooXativa", "KubeConfManager", this);
    ui->setupUi(this);

    this->createActions();
    this->initializeMenus();
    this->initializeToolbars();
    this->initializeApp();

    connect(ui->pushButtonMergeFiles, &QPushButton::clicked, this, &MainWindow::onMergeFilesAction);
    connect(ui->pushButtonHomeMergeFiles, &QPushButton::clicked, this, &MainWindow::onMergeFilesAction);
    connect(this, &MainWindow::contextHasBeenSelected, this, &MainWindow::onContextSelected);
    connect(ui->actionReload, &QAction::triggered, this, &MainWindow::onReloadTriggered);
    connect(ui->listViewContexts, &QListView::doubleClicked, this, &MainWindow::onEditContext);

    uiHasBeenInitialized = true;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializeApp()
{
    kTrace;

    this->showFilesPanel = true;
    this->contextHasBeenEdited = false;

    this->setSaveEnabled(false);

    contextsModel = new QStringListModel(this);
    ui->listViewContexts->setModel(contextsModel);

    this->clearView();

    if (!workingDirectory.isEmpty())
    {
        ui->lineEditWorkingDirectory->setText(workingDirectory);
        this->onReloadTriggered();
        this->ui->listViewFiles->selectionModel()->select(this->ui->listViewFiles->indexAt(QPoint(0, 0)), QItemSelectionModel::Select);
        this->on_listViewFiles_activated(this->ui->listViewFiles->indexAt(QPoint(0, 0)));
    }

    this->on_actionToggleFilesPanel_toggled(this->showFilesPanel);
    this->ui->actionToggleFilesPanel->setChecked(this->showFilesPanel);

    this->ui->groupBoxSelectedContext->setVisible(false);

    this->createTrayIcon();

    if (appSettings->value("ui/start_minimized").toBool())
    {
        qDebug() << "Starting minimized";
        this->showMinimized();
    }

    if (!appSettings->value("ui/start_hidden").toBool())
    {
        this->show();
    }
    else
    {
        this->hide();
    }
}

void MainWindow::initializeToolbars()
{
    kTrace;

    mainToolbar = new QToolBar(tr("Main toolbar"), this);
    mainToolbar->setAllowedAreas(Qt::TopToolBarArea);

    mainToolbar->addAction(actionNewKubeConfig);
    mainToolbar->addAction(actionSaveKubeConfig);
    mainToolbar->addAction(actionSaveAsKubeConfig);
    mainToolbar->addSeparator();
    mainToolbar->addAction(actionShowClusterEditor);
    mainToolbar->addSeparator();
    mainToolbar->addAction(actionShowSettingsDialog);
    mainToolbar->addAction(actionQuitApp);

    this->addToolBar(mainToolbar);

#ifdef QT_DEBUG
    devToolbar = new QToolBar(tr("Developer toolbar"), this);
    devToolbar->addAction(actionDevGoToHome);
    devToolbar->addAction(actionDevGoToMerge);
    devToolbar->addAction(this->actionDevDumpKubeConfig);
    this->addToolBar(Qt::BottomToolBarArea, devToolbar);
#endif
}

void MainWindow::initializeMenus()
{
    kTrace;

    fileMenu = this->menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(this->actionNewKubeConfig);
    fileMenu->addAction(this->actionSaveKubeConfig);
    fileMenu->addAction(this->actionSaveAsKubeConfig);
    fileMenu->addSeparator();
    fileMenu->addAction(actionShowSettingsDialog);
    fileMenu->addAction(actionQuitApp);

#ifdef QT_DEBUG
    devMenu = this->menuBar()->addMenu(tr("&Developer"));
    devMenu->addAction(this->actionDevGoToHome);
    devMenu->addAction(this->actionDevGoToMerge);
    devMenu->addAction(this->actionDevDumpKubeConfig);
#endif
}

void MainWindow::createActions()
{
    kTrace;
    const QIcon iconNewKubeConfig = QIcon::fromTheme("document-new", QIcon(":/icons/document-new"));
    const QIcon iconSaveKubeConfig = QIcon::fromTheme("document-save", QIcon(":/icons/document-save"));
    const QIcon iconSaveAsKubeConfig = QIcon::fromTheme("document-save-as", QIcon(":/icons/document-save-as"));
    const QIcon iconQuit = QIcon::fromTheme("process-stop", QIcon(":/icons/process-stop"));
    const QIcon iconSettings = QIcon::fromTheme("configure", QIcon(":/icons/configure"));
    const QIcon iconGoHome = QIcon::fromTheme("go-home", QIcon(":/icons/go-home"));
    const QIcon iconMerge = QIcon::fromTheme("edit-copy", QIcon(":/icons/edit-copy"));
    const QIcon iconEditClusters = QIcon::fromTheme("edit-clusters", QIcon(":/icons/edit-clusters"));
    const QIcon iconEditUsers = QIcon::fromTheme("edit-users", QIcon(":/icons/edit-users"));

    QIcon iconMergeDev(QPixmap::fromImage(tintImage(QImage("://icons/edit-copy"), QColor(255, 0, 0), 1.0)));
    QIcon iconGoHomeDev(QPixmap::fromImage(tintImage(QImage("://icons/go-home"), QColor(255, 0, 0), 1.0)));
    QIcon iconDumpKubeconfigDev(QPixmap::fromImage(tintImage(QImage("://icons/configure"), QColor(255, 0, 0), 1.0)));

    actionNewKubeConfig = new QAction(iconNewKubeConfig, tr("&New KubeConfig file..."), this);
    actionNewKubeConfig->setShortcuts(QKeySequence::New);
    actionNewKubeConfig->setStatusTip(tr("Create a new config file"));
    connect(actionNewKubeConfig, &QAction::triggered, this, &MainWindow::onNewKubeConfigFile);

    actionQuitApp = new QAction(iconQuit, tr("&Quit..."), this);
    actionQuitApp->setShortcuts(QKeySequence::Quit);
    actionQuitApp->setStatusTip(tr("Close this application and exit"));
    connect(actionQuitApp, &QAction::triggered, this, &MainWindow::exitApplication);

    actionShowSettingsDialog = new QAction(iconSettings, tr("&Preferences..."), this);
    actionShowSettingsDialog->setShortcuts(QKeySequence::Preferences);
    actionShowSettingsDialog->setStatusTip(tr("Setup application preferences and settings"));
    connect(actionShowSettingsDialog, &QAction::triggered, this, &MainWindow::showSettingsDialog);

    actionSaveKubeConfig = new QAction(iconSaveKubeConfig, tr("&Save"), this);
    actionSaveKubeConfig->setShortcuts(QKeySequence::Save);
    actionSaveKubeConfig->setStatusTip(tr("Save current kubeconfig"));

    connect(actionSaveKubeConfig, &QAction::triggered, this, [=]()
            { this->kubeConfig->save(); });

    actionSaveAsKubeConfig = new QAction(iconSaveAsKubeConfig, tr("&Save as..."), this);
    actionSaveAsKubeConfig->setShortcuts(QKeySequence::SaveAs);
    actionSaveAsKubeConfig->setStatusTip(tr("Save current kubeconfig with different name"));

    connect(ui->pushButtonCloneFile, &QPushButton::clicked, this, &MainWindow::onCloneConfigFile);
    connect(ui->pushButtonDeleteFile, &QPushButton::clicked, this, &MainWindow::onDeleteConfigFile);
    connect(ui->pushButtonRenameFile, &QPushButton::clicked, this, &MainWindow::onRenameConfigFile);

    actionShowClusterEditor = new QAction(iconEditClusters, tr("&Edit clusters..."), this);
    actionShowClusterEditor->setStatusTip(tr("Edit clusters on this kubeconfig"));
    connect(actionShowClusterEditor, &QAction::triggered, this, &MainWindow::onEditClusters);

#ifdef QT_DEBUG
    actionDevGoToHome = new QAction(iconGoHomeDev, tr("Stacked widget: home"), this);
    actionDevGoToHome->setStatusTip(tr("Make central widget stack go to home page"));

    connect(actionDevGoToHome, &QAction::triggered, this, [=]()
            { ui->stackedWidget->setCurrentIndex(0); });

    actionDevGoToMerge = new QAction(iconMergeDev, tr("Stacked widget: Merge"), this);
    actionDevGoToMerge->setStatusTip(tr("Make central widget stack go to merge page"));

    connect(actionDevGoToMerge, &QAction::triggered, this, [=]()
            { ui->stackedWidget->setCurrentIndex(2); });

    actionDevDumpKubeConfig = new QAction(iconDumpKubeconfigDev, tr("Dump current KubeConfig"), this);
    actionDevDumpKubeConfig->setStatusTip(tr("Dump current KubeConfig"));

    connect(actionDevDumpKubeConfig, &QAction::triggered, this, [=]()
            { KubeConfig::debug(this->kubeConfig); });
#endif
}

void MainWindow::exitApplication()
{
    kTrace;
    this->close();
    qApp->quit();
}

void MainWindow::onReloadTriggered()
{
    kTrace;

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
    kTrace;
    QStringList keys = appSettings->allKeys();
    qDebug() << "Keys found in config:" << keys.length();

    if (keys.length() == 0)
    {
        QMessageBox::information(this, tr("Configuration not found"), tr("Looks like this is the first time you use this application, please check this settings and have fun!"));
        this->showSettingsDialog();
        return;
    }

    if (appSettings->contains("paths/working_directory"))
    {
        setWorkingDirectory(appSettings->value("paths/working_directory").toString(), false);
    }

    QFileInfo settingsFile(appSettings->fileName());
    QDir settingsDirectory(settingsFile.absoluteDir());

    qDebug() << "Configuration stored at:" << settingsDirectory.path();
}

void MainWindow::setWorkingDirectory(QString path, bool updateSettings = true)
{
    kTrace;
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
    kTrace;
    qDebug() << "Activated on:" << index.data().toString();
    qDebug() << "Current selection count:" << ui->listViewFiles->selectionModel()->selectedIndexes().size();

    if (ui->listViewFiles->selectionModel()->selectedIndexes().size() >= 2)
    {
        ui->pushButtonMergeFiles->setEnabled(true);
        ui->pushButtonDeleteFile->setEnabled(true);
        ui->pushButtonCloneFile->setEnabled(false);
        ui->pushButtonRenameFile->setEnabled(false);
        ui->stackedWidget->setCurrentIndex(0);
        actionShowClusterEditor->setEnabled(false);
        return;
    }
    else if (ui->listViewFiles->selectionModel()->selectedIndexes().size() == 1)
    {
        ui->pushButtonMergeFiles->setEnabled(false);
        ui->pushButtonDeleteFile->setEnabled(true);
        ui->pushButtonCloneFile->setEnabled(true);
        ui->pushButtonRenameFile->setEnabled(true);
        ui->stackedWidget->setCurrentIndex(1);
        actionShowClusterEditor->setEnabled(true);
    }
    else
    {
        ui->pushButtonMergeFiles->setEnabled(false);
        ui->pushButtonDeleteFile->setEnabled(false);
        ui->pushButtonCloneFile->setEnabled(false);
        ui->pushButtonRenameFile->setEnabled(false);
        ui->stackedWidget->setCurrentIndex(0);
        actionShowClusterEditor->setEnabled(false);
        return;
    }

    QString path = workingDirectory;
    path.append("/");
    path.append(index.data().toString());

    KubeParser *parser = new KubeParser(path, this);
    // connect(&parser, &KubeParser::errorLoadingFile, this, &MainWindow::errorLoadingFileParser);
    this->kubeConfig = new KubeConfig(parser->load());

    this->kubeConfigUpdated();
}

void MainWindow::contextModelUpdated(const QStringList contexts)
{
    kTrace;
    contextsModel->setStringList(contexts);
}

void MainWindow::errorLoadingFileParser(const QString message)
{
    kTrace;
    QMessageBox::critical(this, tr("Error parsing file"), message);
}

void MainWindow::showSettingsDialog()
{
    kTrace;
    SettingsForm *settings = new SettingsForm(appSettings, this);
    settings->setModal(true);
    settings->show();

    connect(settings, &SettingsForm::settingsUpdated, this, &MainWindow::clearView);
}

void MainWindow::clearView()
{
    kTrace;
    this->loadSettings();
    this->checkResourcesAndDirectories();

    contextsModel->setStringList(QStringList());

    ui->textEditContextInformation->setMarkdown(
        QString("# Welcome to %1 v%2\n\n---\n* This section is a work in progress...\n\n\n ![Test](https://www.one-beyond.com/wpcms/wp-content/themes/dcsl/assets/images/logo-animated.gif)")
            .arg(APP_NAME, APP_VERSION));
}

bool MainWindow::checkResourcesAndDirectories()
{
    kTrace;
    QDir workingDirectoryDir(this->workingDirectory);

    if (!workingDirectoryDir.exists() || !workingDirectoryDir.isReadable())
    {
        QMessageBox::critical(this, tr("Error accessing kube directory"), QString(tr("Please check if %1 exists and you have read/write access.")).arg(this->workingDirectory));
        this->exitApplication();
        return false;
    }

    if (this->appSettings->contains("paths/disabled_directory"))
    {
        QString disabledDirectoryPath = this->appSettings->value("paths/disabled_directory").toString();
        QDir disabledDirectoryDir(disabledDirectoryPath);

        if (!disabledDirectoryDir.exists())
        {
            QMessageBox msg_box(QMessageBox::Question, tr("Do you want to create this directory?"), QString(tr("Do you want to create this directory to store disabled configurations?\n\n%1")).arg(disabledDirectoryDir.absolutePath()),
                                QMessageBox::Yes | QMessageBox::No);
            msg_box.setButtonText(QMessageBox::Yes, tr("Yes"));
            msg_box.setButtonText(QMessageBox::No, tr("No, disable this feature"));
            if (msg_box.exec() == QMessageBox::Yes)
            {
                if (!disabledDirectoryDir.mkpath(disabledDirectoryPath))
                {
                    QMessageBox::critical(this, tr("Error creating disabled folder"), QString(tr("Unable to create %1, please check that you has permissions to create it and try again.")).arg(disabledDirectoryPath));
                    return false;
                }
            }
            else
            {
                this->appSettings->remove("paths/disabled_directory");
                this->appSettings->sync();
            }
        }
    }

    if (this->appSettings->contains("paths/backup") && this->appSettings->contains("backup/configuration") && this->appSettings->value("backup/configuration").toBool())
    {
        QString backupDirectoryPath = this->appSettings->value("paths/backup").toString();
        QDir backupDirectoryDir(backupDirectoryPath);

        if (!backupDirectoryDir.exists())
        {
            QMessageBox msg_box(QMessageBox::Question, tr("Do you want to create this directory?"), QString(tr("Do you want to create this directory to store backups?\n\n%1")).arg(backupDirectoryDir.absolutePath()),
                                QMessageBox::Yes | QMessageBox::No);
            msg_box.setButtonText(QMessageBox::Yes, tr("Yes"));
            msg_box.setButtonText(QMessageBox::No, tr("No, disable this feature"));
            if (msg_box.exec() == QMessageBox::Yes)
            {
                if (!backupDirectoryDir.mkpath(backupDirectoryPath))
                {
                    QMessageBox::critical(this, tr("Error creating disabled folder"), QString(tr("Unable to create %1, please check that you has permissions to create it and try again.")).arg(backupDirectoryPath));
                    return false;
                }
            }
            else
            {
                this->appSettings->remove("paths/backup");
                this->appSettings->setValue("backup/configuration", false);
                this->appSettings->sync();
            }
        }
    }

    return true;
}

void MainWindow::createTrayIcon()
{
    kTrace;
    systemTrayMenu = new QMenu(this);
    systemTrayMenu->addAction(ui->actionSwitchContext);
    systemTrayMenu->addSeparator();
    systemTrayMenu->addAction(ui->actionQuit);

    appIcon = new QIcon(":/logo/assets/logo.png");

    systemTrayIcon = new QSystemTrayIcon(*appIcon, this);
    systemTrayIcon->setVisible(true);
    systemTrayIcon->setContextMenu(systemTrayMenu);

    connect(systemTrayIcon, &QSystemTrayIcon::activated, this, &MainWindow::on_systemTray_clicked);
}
void MainWindow::on_systemTray_clicked(QSystemTrayIcon::ActivationReason reason)
{
    kTrace;
    switch (reason)
    {
    case QSystemTrayIcon::DoubleClick:
        qDebug() << "SystemTray - Double click";
        break;

    case QSystemTrayIcon::Context:
        qDebug() << "SystemTray - Context requested";
        break;

    case QSystemTrayIcon::Trigger:
        qDebug() << "SystemTray - Trigger";

        if (this->isHidden())
        {
            this->show();
            this->raise();
        }
        else
        {
            this->hide();
        }

        break;

    case QSystemTrayIcon::MiddleClick:
        qDebug() << "SystemTray - Middle click";
        this->on_actionSwitchContext_triggered();
        break;

    case QSystemTrayIcon::Unknown:
        qDebug() << "SystemTray - Unknown activation reason";
        break;
    }
}

void MainWindow::on_actionSwitchContext_triggered()
{
    kTrace;
    ContextSwitcher *switcher = new ContextSwitcher(this->kubeConfig, this->systemTrayIcon);
    switcher->setWindowFlags(Qt::Tool | Qt::Dialog);
    switcher->show();

    connect(this, &MainWindow::closeContextSwitcher, switcher, &ContextSwitcher::closeWindow);
}

void MainWindow::reloadDefaultConfiguration()
{
    kTrace;
}

void MainWindow::on_listViewContexts_activated(const QModelIndex &index)
{
    kTrace;
    qDebug() << "Context activated:" << index.data().toString();
    KubeContextMap *clist = this->kubeConfig->contexts();
    qDebug() << "[P] Contexts:" << clist->size();

    for (auto i = this->kubeConfig->contexts()->begin(), end = this->kubeConfig->contexts()->end(); i != end; ++i)
    {
        KubeContext current = *i;
        if (current.name == index.data().toString())
        {
            this->selectedContext = current;
            emit contextHasBeenSelected();
            return;
        }
    }
}

void MainWindow::kubeConfigUpdated()
{
    kTrace;
    qDebug() << "KubeConfig has been updated";
    qDebug() << "[P] Contexts:" << this->kubeConfig->contexts()->size();

    KubeConfigUtils kubeUtils(this->kubeConfig, this);

    QStringList contextModel = kubeUtils.getContextsStringList();
    this->contextModelUpdated(contextModel);

#ifdef QT_DEBUG
    if (appSettings->contains("dev/save_all_changes") && appSettings->value("dev/save_all_changes").toBool())
    {
        QString filepath("/tmp/current-kubeconfig-tmp.yaml");
        QString test2(KubeParser::dumpConfig(this->kubeConfig));
        QFile file(filepath);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        QTextStream out(&file);
        out << test2;

        qDebug() << "[DEV] Current kubeconfig stored at:" << filepath;
    }
#endif
}

void MainWindow::onContextSelected()
{
    kTrace;
    qDebug() << "New context selected:" << this->selectedContext.name;
    ui->lineEditContextName->setText(this->selectedContext.name);
    ui->lineEditContextName->setReadOnly(true);
    QStringListModel *contextClustersModel = new QStringListModel(this->kubeConfig->clustersNames());
    QStringListModel *userClustersModel = new QStringListModel(this->kubeConfig->usersNames());

    ui->comboBoxCluster->setModel(contextClustersModel);
    ui->comboBoxCluster->setEnabled(false);
    ui->comboBoxUser->setModel(userClustersModel);
    ui->comboBoxUser->setEnabled(false);

    this->contextHasBeenEdited = false;

    this->ui->groupBoxSelectedContext->setVisible(true);
}

void MainWindow::updateContextInformationText()
{
    kTrace;
    QString text(QString("Cluster %1\n    User: %2\n").arg(this->selectedContext.name, this->selectedContext.name));
    ui->textEditContextInformation->setMarkdown(text);
    ui->lineEditContextName->setText(this->selectedContext.name);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Escape)
        {
            // handle escape to close context switch window
            if (obj->objectName() == "ContextSwitcherWindow")
            {
                emit closeContextSwitcher();
            }
        }
        // qDebug() << "key " << keyEvent->key() << "from" << obj;
    }

    return QObject::eventFilter(obj, event);
}

void MainWindow::on_actionToggleFilesPanel_toggled(bool showPanel)
{
    kTrace;
    if (showPanel)
    {
        this->ui->dockWidgetConfigurationFiles->show();
    }
    else
    {
        this->ui->dockWidgetConfigurationFiles->hide();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    kTrace;
    event->acceptProposedAction();

    qDebug() << "Drop Event received: " << event->mimeData()->formats().join(",");
    qDebug() << "Drop content: " << event->mimeData()->text();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    kTrace;
    if (event->mimeData()->hasFormat("text/plain"))
        event->acceptProposedAction();

    qDebug() << "Drop Event looks like" << event->mimeData()->formats().join(",");

    if (event->mimeData()->hasUrls())
        qDebug() << "Has urls";

    if (event->mimeData()->hasText())
        qDebug() << "Has text";
}

void MainWindow::onMergeFilesAction()
{
    kTrace;
    KubeConfigMerger *mergerWidget = new KubeConfigMerger(ui->stackedWidget);
    ui->stackedWidget->setCurrentIndex(2);
    ui->stackedWidget->setCurrentWidget(mergerWidget);
}

void MainWindow::onNewKubeConfigFile()
{
    kTrace;
    QFile defaultConfig(QString("%1/config").arg(this->workingDirectory));
    CreateNewKubeConfigDialog *dialog = new CreateNewKubeConfigDialog(this->workingDirectory, defaultConfig.exists(), this);
    dialog->setWindowFlags(Qt::Tool | Qt::Dialog);
    dialog->show();

    connect(dialog, &CreateNewKubeConfigDialog::createFile, this, [=](QString filepath)
            {
        qDebug() << "Creating new config file:" << filepath;
        KubeConfig *newConfig = KubeParser::createEmptyConfig();        
        QString kubeDump = KubeParser::dumpConfig(newConfig);
        qDebug() << "Config:" << kubeDump;
        QFile newConfigFile(filepath);

        if (!newConfigFile.open(QIODevice::ReadWrite | QIODevice::Text)){
            //@todo show an error
        }else{
            newConfigFile.write(kubeDump.toUtf8());
            newConfigFile.setPermissions(QFileDevice::ReadUser | QFileDevice::WriteUser );
            this->onReloadTriggered();
        }

        dialog->hide();
        delete dialog; });
}

void MainWindow::onEditContext(const QModelIndex &index)
{
    kTrace;
    qDebug() << "Context to edit:" << index.data().toString();
    qDebug() << "Contexts:" << this->kubeConfig->contexts()->size();

    KubeConfigUtils kubeUtils(this->kubeConfig, this);
    auto *context = kubeUtils.getContextByName(index.data().toString());

    qDebug() << context->name;
    ContextEditor contextEditor(context, this->kubeConfig, false, this);
    contextEditor.setModal(true);

    connect(&contextEditor, &ContextEditor::contextSaved, this, [=](KubeContext *newContext)
            { 
                qDebug() << "Context has been edited"; 
                this->contextHasBeenEdited = true;
                KubeConfig::debug(this->kubeConfig);
                this->kubeConfig->updateContext(context->name, newContext ); 
                emit pendingChangesToSave();
                this->setSaveEnabled(true);
                KubeConfig::debug(this->kubeConfig); 
                this->kubeConfigUpdated(); });

    contextEditor.exec();
}

void MainWindow::setSaveEnabled(bool state)
{
    this->actionSaveKubeConfig->setEnabled(state);
    this->actionSaveAsKubeConfig->setEnabled(state);
}

void MainWindow::onCloneConfigFile()
{
    kTrace;

    int selectedIndexes = ui->listViewFiles->selectionModel()->selectedIndexes().size();
    qDebug() << "Current selection count:" << selectedIndexes;

    if (selectedIndexes == 0)
    {
        QMessageBox::critical(this, tr("Error"), tr("You must select a file to clone"));
        return;
    }

    if (selectedIndexes > 1)
    {
        QMessageBox::critical(this, tr("Error"), tr("You can only clone one file at a time"));
        return;
    }

    QString selectedIndexText = ui->listViewFiles->selectionModel()->selectedIndexes().at(0).data().toString();
    qDebug() << "Selected index:" << selectedIndexText;
    QString selectedFilePath = QString("%1/%2").arg(this->workingDirectory, selectedIndexText);

    QString newConfigFilepath = QFileDialog::getSaveFileName(this, tr("Save kubeconfig file"), this->workingDirectory, tr("Kubeconfig files (*)"));
    qDebug() << "New config file:" << newConfigFilepath;

    if (newConfigFilepath.isEmpty())
    {
        QMessageBox::warning(this, tr("Error"), tr("You must provide a valid file name"));
        return;
    }

    qDebug() << "Copying file:" << selectedFilePath << "to" << newConfigFilepath;

    if (!QFile::copy(selectedFilePath, newConfigFilepath))
    {
        QMessageBox::critical(this, tr("Error"), tr("Error copying file"));
        return;
    }

    QMessageBox::information(this, tr("Success"), tr("File copied successfully"));
    this->onReloadTriggered();
}

void MainWindow::onRenameConfigFile()
{
    kTrace;

    int selectedIndexes = ui->listViewFiles->selectionModel()->selectedIndexes().size();
    qDebug() << "Current selection count:" << selectedIndexes;

    if (selectedIndexes == 0)
    {
        QMessageBox::critical(this, tr("Error"), tr("You must select a file to rename"));
        return;
    }

    if (selectedIndexes > 1)
    {
        QMessageBox::critical(this, tr("Error"), tr("You can only rename one file at a time"));
        return;
    }

    QString selectedIndexText = ui->listViewFiles->selectionModel()->selectedIndexes().at(0).data().toString();
    qDebug() << "Selected index:" << selectedIndexText;
    QString selectedFilePath = QString("%1/%2").arg(this->workingDirectory, selectedIndexText);

    QString newConfigFilepath = QFileDialog::getSaveFileName(this, tr("Rename kubeconfig file"), this->workingDirectory, tr("Kubeconfig files (*)"));
    qDebug() << "New config file path:" << newConfigFilepath;

    if (newConfigFilepath.isEmpty())
    {
        QMessageBox::warning(this, tr("Error"), tr("You must provide a valid file name"));
        return;
    }

    qDebug() << "Renaming file from:" << selectedFilePath << "to" << newConfigFilepath;
    if (!QFile::rename(selectedFilePath, newConfigFilepath))
    {
        QMessageBox::critical(this, tr("Error"), tr("Error renaming file"));
        return;
    }

    QMessageBox::information(this, tr("Success"), tr("File renamed successfully"));
    this->onReloadTriggered();
}

void MainWindow::onDeleteConfigFile()
{
    kTrace;

    int selectedIndexes = ui->listViewFiles->selectionModel()->selectedIndexes().size();
    qDebug() << "Current selection count:" << selectedIndexes;

    if (selectedIndexes == 0)
    {
        QMessageBox::critical(this, tr("Error"), tr("You must select a file to remove"));
        return;
    }

    QString title;
    QString message;

    if (selectedIndexes == 1)
    {
        title = tr("Do you want to remove this file?");
        message = tr("Do you want to remove %1 file?").arg(selectedIndexes);
    }
    else
    {
        title = tr("Do you want to remove this files?");
        message = tr("Do you want to remove %1 files?").arg(selectedIndexes);
    }

    message.append("\nWarning: This action cannot be undone!");

    QMessageBox msg_box(QMessageBox::Question, title, message,
                        QMessageBox::Yes | QMessageBox::No);
    msg_box.setButtonText(QMessageBox::Yes, tr("Yes"));
    msg_box.setButtonText(QMessageBox::No, tr("No"));
    if (msg_box.exec() == QMessageBox::Yes)
    {
        for (const auto &index : ui->listViewFiles->selectionModel()->selectedIndexes())
        {
            QString selectedIndexText = index.data().toString();
            qDebug() << "Selected index:" << selectedIndexText;
            QString selectedFilePath = QString("%1/%2").arg(this->workingDirectory, selectedIndexText);

            qDebug() << "Removing file:" << selectedFilePath;
            if (!QFile::remove(selectedFilePath))
            {
                QMessageBox::critical(this, tr("Error"), QString(tr("Error removing file %1")).arg(selectedFilePath));
                return;
            }
        }

        QMessageBox::information(this, tr("Success"), tr("File(s) removed successfully"));
        this->onReloadTriggered();
    }
    else
    {
        return;
    }
}

void MainWindow::onEditClusters()
{
    kTrace;
    qDebug() << "Edit clusters";

    ClusterEditor *clusterEditor = new ClusterEditor(this->kubeConfig, this);
    clusterEditor->setModal(true);
    clusterEditor->show();
}
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

    uiHasBeenInitialized = true;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializeApp()
{
    this->showFilesPanel = true;

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
    mainToolbar = new QToolBar(tr("Main toolbar"), this);
    mainToolbar->setAllowedAreas(Qt::TopToolBarArea);

    mainToolbar->addAction(actionNewKubeConfig);
    mainToolbar->addSeparator();
    mainToolbar->addAction(actionShowSettingsDialog);
    mainToolbar->addAction(actionQuitApp);

    this->addToolBar(mainToolbar);

#ifdef QT_DEBUG
    devToolbar = new QToolBar(tr("Developer toolbar"), this);
    devToolbar->addAction(actionDevGoToHome);
    devToolbar->addAction(actionDevGoToMerge);
    this->addToolBar(Qt::BottomToolBarArea, devToolbar);
#endif
}

void MainWindow::initializeMenus()
{
    fileMenu = this->menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(this->actionNewKubeConfig);
    fileMenu->addSeparator();
    fileMenu->addAction(actionShowSettingsDialog);
    fileMenu->addAction(actionQuitApp);

#ifdef QT_DEBUG
    devMenu = this->menuBar()->addMenu(tr("&Developer"));
    devMenu->addAction(this->actionDevGoToHome);
    devMenu->addAction(this->actionDevGoToMerge);
#endif
}

void MainWindow::createActions()
{
    const QIcon iconNewKubeConfig = QIcon::fromTheme("document-new", QIcon(":/icons/document-new"));
    const QIcon iconQuit = QIcon::fromTheme("process-stop", QIcon(":/icons/process-stop"));
    const QIcon iconSettings = QIcon::fromTheme("configure", QIcon(":/icons/configure"));
    const QIcon iconGoHome = QIcon::fromTheme("go-home", QIcon(":/icons/go-home"));
    const QIcon iconMerge = QIcon::fromTheme("edit-copy", QIcon(":/icons/edit-copy"));

    QIcon iconMergeDev(QPixmap::fromImage(tintImage(QImage("://icons/edit-copy"), QColor(255, 0, 0), 1.0)));
    QIcon iconGoHomeDev(QPixmap::fromImage(tintImage(QImage("://icons/go-home"), QColor(255, 0, 0), 1.0)));

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

#ifdef QT_DEBUG
    actionDevGoToHome = new QAction(iconGoHomeDev, tr("Stacked widget: home"), this);
    actionDevGoToHome->setStatusTip(tr("Make central widget stack go to home page"));

    connect(actionDevGoToHome, &QAction::triggered, this, [=]()
            { ui->stackedWidget->setCurrentIndex(0); });

    actionDevGoToMerge = new QAction(iconMergeDev, tr("Stacked widget: Merge"), this);
    actionDevGoToMerge->setStatusTip(tr("Make central widget stack go to merge page"));

    connect(actionDevGoToMerge, &QAction::triggered, this, [=]()
            { ui->stackedWidget->setCurrentIndex(2); });
#endif
}

void MainWindow::exitApplication()
{
    this->close();
    qApp->quit();
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

void MainWindow::onReloadTriggered()
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
    qDebug() << "Current selection count:" << ui->listViewFiles->selectionModel()->selectedIndexes().size();

    if (ui->listViewFiles->selectionModel()->selectedIndexes().size() >= 2)
    {
        ui->pushButtonMergeFiles->setEnabled(true);
        ui->pushButtonDeleteFile->setEnabled(true);
        ui->pushButtonCloneFile->setEnabled(false);
        ui->pushButtonRenameFile->setEnabled(false);
        ui->stackedWidget->setCurrentIndex(0);
        return;
    }
    else if (ui->listViewFiles->selectionModel()->selectedIndexes().size() == 1)
    {
        ui->pushButtonMergeFiles->setEnabled(false);
        ui->pushButtonDeleteFile->setEnabled(true);
        ui->pushButtonCloneFile->setEnabled(true);
        ui->pushButtonRenameFile->setEnabled(true);
        ui->stackedWidget->setCurrentIndex(1);
    }
    else
    {
        ui->pushButtonMergeFiles->setEnabled(false);
        ui->pushButtonDeleteFile->setEnabled(false);
        ui->pushButtonCloneFile->setEnabled(false);
        ui->pushButtonRenameFile->setEnabled(false);
        ui->stackedWidget->setCurrentIndex(0);
        return;
    }

    QString path = workingDirectory;
    path.append("/");
    path.append(index.data().toString());

    KubeParser parser(path, this);

    connect(&parser, &KubeParser::errorLoadingFile, this, &MainWindow::errorLoadingFileParser);
    // connect(&parser, &KubeParser::contextsLoaded, this, &MainWindow::contextModelUpdated);
    connect(&parser, &KubeParser::kubeConfigLoaded, this, &MainWindow::kubeConfigUpdated);

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

void MainWindow::showSettingsDialog()
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

    this->ui->actionEditClusters->setEnabled(false);
    this->ui->actionEditUsers->setEnabled(false);

    ui->textEditContextInformation->setMarkdown(
        QString("# Welcome to %1 v%2\n\n---\n* This section is a work in progress...\n\n\n ![Test](https://www.one-beyond.com/wpcms/wp-content/themes/dcsl/assets/images/logo-animated.gif)")
            .arg(APP_NAME, APP_VERSION));
}

bool MainWindow::checkResourcesAndDirectories()
{
    QDir workingDirectory(this->workingDirectory);

    if (!workingDirectory.exists() || !workingDirectory.isReadable())
    {
        QMessageBox::critical(this, tr("Error accessing kube directory"), QString(tr("Please check if %1 exists and you have read/write access.")).arg(this->workingDirectory));
        this->exitApplication();
        return false;
    }

    if (this->appSettings->contains("paths/disabled_directory"))
    {
        QString disabledDirectoryPath = this->appSettings->value("paths/disabled_directory").toString();
        QDir disabledDirectory(disabledDirectoryPath);

        if (!disabledDirectory.exists())
        {
            QMessageBox msg_box(QMessageBox::Question, tr("Do you want to create this directory?"), QString(tr("Do you want to create this directory to store disabled configurations?\n\n%1")).arg(disabledDirectory.absolutePath()),
                                QMessageBox::Yes | QMessageBox::No);
            msg_box.setButtonText(QMessageBox::Yes, tr("Yes"));
            msg_box.setButtonText(QMessageBox::No, tr("No, disable this feature"));
            if (msg_box.exec() == QMessageBox::Yes)
            {
                if (!disabledDirectory.mkpath(disabledDirectoryPath))
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
    return true;
}

void MainWindow::createTrayIcon()
{
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
    ContextSwitcher *switcher = new ContextSwitcher(this->kubeConfig, this->systemTrayIcon);
    switcher->setWindowFlags(Qt::Tool | Qt::Dialog);
    switcher->show();

    connect(this, &MainWindow::closeContextSwitcher, switcher, &ContextSwitcher::closeWindow);
}

void MainWindow::reloadDefaultConfiguration()
{
}

void MainWindow::on_listViewContexts_activated(const QModelIndex &index)
{
    qDebug() << "Context activated:" << index.data().toString();
    //    qDebug() << "Contexts loaded:" << this->contexts->size();
    //    for (auto i = this->contexts->begin(), end = this->contexts->end(); i != end; ++i)
    //    {
    //        KubeContext current = *i;
    //        if (current.name == index.data().toString())
    //        {
    //            this->selectedContext = current;
    //            emit contextHasBeenSelected();
    //            return;
    //        }
    //    }
}

void MainWindow::kubeConfigUpdated(KubeConfig *kConfig)
{
    qDebug() << "KubeConfig has been updated";
    this->kubeConfig = kConfig;
    this->kubeUtils = new KubeConfigUtils(this->kubeConfig, this);

    // KubeContext *test = this->kubeUtils->getContextByName(this->kubeUtils->getCurrentContext());
    // qDebug() << "\tSelected context:" << test->name;

    this->ui->actionEditClusters->setEnabled(true);
    this->ui->actionEditUsers->setEnabled(true);

    QStringList contextModel = this->kubeUtils->getContextsStringList();
    this->contextModelUpdated(contextModel);

#ifdef QT_DEBUG
    QString test2(KubeParser::dumpConfig(this->kubeConfig));
    QFile file("/tmp/current-kubeconfig-tmp.yaml");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << test2;
#endif
}

void MainWindow::onContextSelected()
{
    qDebug() << "New context selected:" << this->selectedContext.name;
    this->updateContextInformationText();
}

void MainWindow::updateContextInformationText()
{
    QString text(QString("Cluster %1\n    User: %2\n").arg(this->selectedContext.name, this->selectedContext.name));
    ui->textEditContextInformation->setMarkdown(text);
    ui->lineEditContextName->setText(this->selectedContext.name);
}

void MainWindow::on_actionEditClusters_triggered()
{
    ClusterEditor *editorWidget = new ClusterEditor(this->kubeConfig, this);
    editorWidget->setWindowFlags(Qt::Tool | Qt::Dialog);
    editorWidget->show();
}

void MainWindow::on_actionEditUsers_triggered()
{
}

void MainWindow::on_actionAbout_triggered()
{
}

void MainWindow::on_actionNew_KubeConfig_file_triggered()
{
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
    if (showPanel)
    {
        this->ui->dockWidgetConfigurationFiles->show();
    }
    else
    {
        this->ui->dockWidgetConfigurationFiles->hide();
    }
}

void MainWindow::on_listViewContexts_doubleClicked(const QModelIndex &index)
{
    ContextEditor *editor = new ContextEditor(this->kubeUtils->getContextByName(index.data().toString()), this->kubeConfig, this);
    editor->setWindowFlags(Qt::Tool | Qt::Dialog);
    editor->show();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    event->acceptProposedAction();

    qDebug() << "Drop Event received: " << event->mimeData()->formats().join(",");
    qDebug() << "Drop content: " << event->mimeData()->text();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
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
    KubeConfigMerger *mergerWidget = new KubeConfigMerger(ui->stackedWidget);
    ui->stackedWidget->setCurrentIndex(2);
    ui->stackedWidget->setCurrentWidget(mergerWidget);
}

void MainWindow::onNewKubeConfigFile()
{
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
            this->onReloadTriggered();
        }

        dialog->hide();
        delete dialog; });
}

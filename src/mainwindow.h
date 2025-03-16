#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QToolBar>
#include <QMainWindow>
#include <QSettings>
#include <QMimeData>
#include <QDebug>
#include <QStringListModel>
#include <QMenu>
#include <QMenuBar>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QWindow>
#include <QEvent>
#include <QKeyEvent>

#include "kubeconfigutils.h"
#include "KubeConfManager.h"
#include "clustereditor.h"
#include "kubeconfigmerger.h"
#include "contextswitcher.h"
#include "createnewkubeconfigdialog.h"
#include "tint.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    //    void dragMoveEvent(QDragMoveEvent *event) override;
    //    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
private slots:
    void exitApplication();
    void on_listViewFiles_activated(const QModelIndex &index);

    void errorLoadingFileParser(const QString message);

    void contextModelUpdated(const QStringList contexts);
    void kubeConfigUpdated();

    void loadSettings();
    void clearView();
    void initializeApp();
    void reloadDefaultConfiguration();
    void on_actionSwitchContext_triggered();
    void on_listViewContexts_activated(const QModelIndex &index);
    void onContextSelected();
    void updateContextInformationText();
    void on_systemTray_clicked(QSystemTrayIcon::ActivationReason reason);
    void on_actionToggleFilesPanel_toggled(bool arg1);
    void onMergeFilesAction();
    void onNewKubeConfigFile();
    void onReloadTriggered();
    void showSettingsDialog();
    void onEditContext(const QModelIndex &index);
    void setSaveEnabled(bool);
    void onCloneConfigFile();
    void onRenameConfigFile();
    void onDeleteConfigFile();
    void onEditClusters();
    void onRemoveSelectedContext();

signals:
    void contextHasBeenSelected();
    void pendingChangesToSave();
    void closeContextSwitcher();

private:
    void setWorkingDirectory(QString, bool);
    bool checkResourcesAndDirectories();

    void createTrayIcon();
    void createActions();

    void initializeToolbars();
    void initializeMenus();

    // ui
    Ui::MainWindow *ui;
    bool uiHasBeenInitialized;

    // menus
    QMenu *fileMenu;

    // actions
    QAction *actionNewKubeConfig;
    QAction *actionSaveKubeConfig;
    QAction *actionSaveAsKubeConfig;
    QAction *actionQuitApp;
    QAction *actionShowSettingsDialog;
    QAction *actionShowClusterEditor;

    // toolbaes
    QToolBar *mainToolbar;
    QToolBar *contextToolbar;

    // settings
    QSettings *appSettings;
    QString workingDirectory;
    QString disabledDirectory;
    QString backupDirectory;
    QString devChangesDirectory;

    // Models
    QStringListModel *contextsModel;

    // System tray
    QMenu *systemTrayMenu;
    QSystemTrayIcon *systemTrayIcon;
    QIcon *appIcon;

    KubeContext selectedContext;
    KubeConfig *kubeConfig;

    bool showFilesPanel;
    bool contextHasBeenEdited;

    /**
     * developer stuff
     */
#ifdef QT_DEBUG
    QToolBar *devToolbar;
    QMenu *devMenu;

    QAction *actionDevGoToHome;
    QAction *actionDevGoToMerge;
    QAction *actionDevDumpKubeConfig;

#endif
};
#endif // MAINWINDOW_H

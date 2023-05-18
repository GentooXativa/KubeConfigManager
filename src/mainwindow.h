#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QDebug>
#include <QStringListModel>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QWindow>
#include <QEvent>
#include <QKeyEvent>

#include "kubeconfigutils.h"
#include "KubeConfManager.h"
#include "clustereditor.h"
#include "contextswitcher.h"

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
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void on_actionQuit_triggered();
    void on_toolButtonWorkingDirectory_clicked();
    void on_actionReload_triggered();
    void on_listViewFiles_activated(const QModelIndex &index);

    void errorLoadingFileParser(const QString message);

    void contextModelUpdated(const QStringList contexts);
    void kubeConfigUpdated(KubeConfig *kConfig);

    void on_actionSettings_triggered();
    void loadSettings();
    void clearView();
    void initializeApp();
    void reloadDefaultConfiguration();

    void on_actionSwitchContext_triggered();
    void on_listViewContexts_activated(const QModelIndex &index);

    void onContextSelected();
    void updateContextInformationText();

    void on_actionEditClusters_triggered();

    void on_actionEditUsers_triggered();

    void on_systemTray_clicked(QSystemTrayIcon::ActivationReason reason);

    void on_actionAbout_triggered();

    void on_actionNew_KubeConfig_file_triggered();

    void on_actionToggleFilesPanel_toggled(bool arg1);

    void on_listViewContexts_doubleClicked(const QModelIndex &index);

signals:
    void contextHasBeenSelected();
    void closeContextSwitcher();

private:
    void setWorkingDirectory(QString, bool);
    bool checkResourcesAndDirectories();
    void createTrayIcon();

    Ui::MainWindow *ui;
    bool uiHasBeenInitialized;
    QSettings *appSettings;

    QString workingDirectory;
    QString disabledDirectory;
    QString backupDirectory;
    QString devChangesDirectory;

    QStringListModel *contextsModel;
    QMenu *systemTrayMenu;
    QSystemTrayIcon *systemTrayIcon;
    QIcon *appIcon;

    KubeContextList *contexts;
    KubeContext selectedContext;
    KubeConfig *kubeConfig;
    KubeConfigUtils *kubeUtils;

    bool showFilesPanel;
};
#endif // MAINWINDOW_H

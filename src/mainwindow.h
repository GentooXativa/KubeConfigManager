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
    void reloadDefaultConfiguration();

    void on_actionSwitchContext_triggered();
    void on_listViewContexts_activated(const QModelIndex &index);

    void onContextSelected();
    void updateContextInformationText();

    void on_actionEditClusters_triggered();

    void on_actionEditUsers_triggered();

    void on_systemTray_clicked(QSystemTrayIcon::ActivationReason reason);

signals:
    void contextHasBeenSelected();

private:
    void setWorkingDirectory(QString, bool);
    bool checkResourcesAndDirectories();
    void createTrayIcon();

    Ui::MainWindow *ui;
    bool uiHasBeenInitialized;
    QSettings *appSettings;
    QString workingDirectory;

    QStringListModel *contextsModel;
    QMenu *systemTrayMenu;
    QSystemTrayIcon *systemTrayIcon;
    QIcon *appIcon;

    KubeContextList *contexts;
    KubeContext selectedContext;
    KubeConfig *kubeConfig;
    KubeConfigUtils *kubeUtils;
};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QDebug>
#include <QStringListModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
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
    
    void clusterModelUpdated(const QStringList clusters );
    void contextModelUpdated(const QStringList contexts );
    void userModelUpdated(const QStringList users );

private:
    void    loadSettings();
    void    setWorkingDirectory(QString, bool);

    Ui::MainWindow  *ui;
    bool    uiHasBeenInitialized;
    QSettings       *appSettings;
    QString     workingDirectory;

    QStringListModel    *clustersModel;
    QStringListModel    *contextsModel;
    QStringListModel    *usersModel;
};
#endif // MAINWINDOW_H

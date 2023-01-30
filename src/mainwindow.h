#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QDebug>
#include <QStringListModel>

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

    void on_actionSettings_triggered();
    void loadSettings();
    void clearView();

private:
    void setWorkingDirectory(QString, bool);
    bool checkResourcesAndDirectories();

    Ui::MainWindow *ui;
    bool uiHasBeenInitialized;
    QSettings *appSettings;
    QString workingDirectory;

    QStringListModel *contextsModel;
};
#endif // MAINWINDOW_H

#ifndef CREATENEWKUBECONFIGDIALOG_H
#define CREATENEWKUBECONFIGDIALOG_H

#include <QDebug>
#include <QFile>
#include <QWidget>

namespace Ui {
class CreateNewKubeConfigDialog;
}

class CreateNewKubeConfigDialog : public QWidget
{
    Q_OBJECT

public:
    explicit CreateNewKubeConfigDialog( QString currentFolder, bool defaultConfigExists, QWidget *parent = nullptr);
    ~CreateNewKubeConfigDialog();

private slots:
    void checkFile();

signals:
    void    createFile(QString);

private:
    Ui::CreateNewKubeConfigDialog *ui;
};

#endif // CREATENEWKUBECONFIGDIALOG_H

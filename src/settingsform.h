#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QDialog>
#include <QSettings>

namespace Ui
{
    class SettingsForm;
}

class SettingsForm : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsForm(QSettings *appSettings, QWidget *parent = nullptr);
    ~SettingsForm();

signals:
    void settingsUpdated();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_toolButtonWorkingDirectory_triggered(QAction *arg1);

    void on_toolButtonDisabledFolderPath_triggered(QAction *arg1);

    void on_pushButtonKubeCtlCheck_clicked();

    void on_checkBoxStartHidden_toggled(bool checked);

    void on_checkBoxStartMinimized_toggled(bool checked);

private:
    void updateUI();
    Ui::SettingsForm *ui;
    QSettings *appSettings;
};

#endif // SETTINGSFORM_H

#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QDialog>
#include <QLineEdit>
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
    void onSaveConfig();
    void onReject();
    void onTryToFindKubectl();
    void onSearchFolder(QLineEdit *target);

private:
    void updateUI();
    Ui::SettingsForm *ui;
    QSettings *appSettings;
};

#endif // SETTINGSFORM_H

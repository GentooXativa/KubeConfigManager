#ifndef CONTEXTSWITCHER_H
#define CONTEXTSWITCHER_H

#include <QSystemTrayIcon>
#include <QWidget>

#include "KubeConfManager.h"
#include "kubeconfigutils.h"

namespace Ui
{
    class ContextSwitcher;
}

class ContextSwitcher : public QWidget
{
    Q_OBJECT

public:
    explicit ContextSwitcher(KubeConfig *config, QSystemTrayIcon *trayIcon, QWidget *parent = nullptr);
    ~ContextSwitcher();

private slots:
    void on_listView_activated(const QModelIndex &index);

private:
    Ui::ContextSwitcher *ui;
    KubeConfig *currentKubeConfig;
    QSystemTrayIcon *trayIcon;
};

#endif // CONTEXTSWITCHER_H

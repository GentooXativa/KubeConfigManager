#include <QStringListModel>
#include <QDebug>

#include "contextswitcher.h"
#include "ui_contextswitcher.h"

ContextSwitcher::ContextSwitcher(KubeConfig *config, QSystemTrayIcon *trayIcon, QWidget *parent) : QWidget(parent),
                                                                                                   ui(new Ui::ContextSwitcher)
{
    ui->setupUi(this);
    this->currentKubeConfig = config;
    this->trayIcon = trayIcon;
    KubeConfigUtils kUtils(this->currentKubeConfig, this);

    QStringListModel *contextModel = new QStringListModel(kUtils.getContextsStringList());
    this->ui->listView->setModel(contextModel);

    if (!config->currentContext.isEmpty() && !config->currentContext.isNull())
    {
        int contextIndex = contextModel->stringList().indexOf(config->currentContext);
        this->ui->listView->selectionModel()->select(contextModel->index(contextIndex), QItemSelectionModel::Select);
    }
}

ContextSwitcher::~ContextSwitcher()
{
    delete ui;
}

void ContextSwitcher::on_listView_activated(const QModelIndex &index)
{
    qDebug() << "New context selected";
    this->trayIcon->showMessage(tr("New context selected"), QString(tr("%1 has been selected as active context.")).arg(true), QSystemTrayIcon::Information);
    this->close();
}

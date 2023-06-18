#include <QStringListModel>
#include <QDebug>
#include <QProcess>
#include <QProcessEnvironment>

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

    if (!config->currentContext().isEmpty() && !config->currentContext().isNull())
    {
        int contextIndex = contextModel->stringList().indexOf(config->currentContext());
        this->ui->listView->selectionModel()->select(contextModel->index(contextIndex), QItemSelectionModel::Select);
    }
}

ContextSwitcher::~ContextSwitcher()
{
    delete ui;
    delete this->currentKubeConfig;
    delete this->trayIcon;
}

void ContextSwitcher::on_listView_activated(const QModelIndex &index)
{
    QProcess kubectl;
    QStringList arguments;

    arguments << "config"
              << "use-context" << index.data().toString();

    qDebug() << "kubectl" << arguments;

    kubectl.start("kubectl", arguments, QIODevice::ReadOnly);

    if (!kubectl.waitForFinished())
        return;

    QByteArray result = kubectl.readAll();

    qDebug() << "New context selected:" << QString::fromStdString(result.toStdString());
    this->trayIcon->showMessage(tr("New context selected"), QString(tr("%1 has been selected as active context.")).arg(index.data().toString()), QSystemTrayIcon::Information);
    this->close();
}

void ContextSwitcher::closeWindow()
{
    this->close();
}

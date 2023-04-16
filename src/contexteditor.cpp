#include "kubeparser.h"
#include "contexteditor.h"
#include "ui_contexteditor.h"

ContextEditor::ContextEditor(KubeContext *context, KubeConfig *kConfig, QWidget *parent) : QWidget(parent),
                                                                                           ui(new Ui::ContextEditor)
{
    this->context = context;
    this->config = kConfig;

    ui->setupUi(this);

    KubeConfigUtils utils(this->config, this);

    QStringList clusterList = utils.getClustersStringList();
    QStringListModel *clusterModel = new QStringListModel(clusterList);
    this->ui->comboBoxClusters->setModel(clusterModel);

    QStringList userList = utils.getUsersStringList();
    qDebug() << "received users:" << userList;
    QStringListModel *userModel = new QStringListModel(userList);
    this->ui->comboBoxUsers->setModel(userModel);

    qDebug() << "Current context:" << this->context->name;
    qDebug() << "\tUser    :" << this->context->user->name;
    qDebug() << "\tCluster :" << this->context->cluster->name;
}

ContextEditor::~ContextEditor()
{
    delete ui;
    delete this->context;
    delete this->config;
}

#include "kubeparser.h"
#include "contexteditor.h"
#include "ui_contexteditor.h"

ContextEditor::ContextEditor(KubeContext *context, KubeConfig *kConfig, QWidget *parent) : QDialog(parent),
                                                                                           ui(new Ui::ContextEditor)
{
    this->context = context;
    this->config = kConfig;

    ui->setupUi(this);

    KubeConfigUtils utils(this->config, this);

    this->ui->lineEditContextName->setText(this->context->name);

    if (!this->context->clusterNamespace.isEmpty())
    {
        this->ui->lineEditNamespace->setText(this->context->clusterNamespace);
    }

    QStringList clusterList = utils.getClustersStringList();
    QStringListModel *clusterModel = new QStringListModel(clusterList);
    this->ui->comboBoxClusters->setModel(clusterModel);

    QStringList userList = utils.getUsersStringList();
    QStringListModel *userModel = new QStringListModel(userList);
    this->ui->comboBoxUsers->setModel(userModel);

    if (this->ui->comboBoxClusters->findText(this->context->cluster->name))
        this->ui->comboBoxClusters->setCurrentIndex(this->ui->comboBoxClusters->findText(this->context->cluster->name));

    if (this->ui->comboBoxUsers->findText(this->context->user->name))
        this->ui->comboBoxUsers->setCurrentIndex(this->ui->comboBoxUsers->findText(this->context->user->name));

    connect(this->ui->lineEditContextName, &QLineEdit::textChanged, this, &ContextEditor::checkContextName);
}

ContextEditor::~ContextEditor()
{
    delete ui;
    delete this->context;
    delete this->config;
}

void ContextEditor::on_pushButtonSave_clicked()
{
}

void ContextEditor::on_pushButtonCancel_clicked()
{
    this->close();
}

void ContextEditor::checkContextName()
{
    if (this->ui->lineEditContextName->text().isEmpty())
    {

        this->ui->labelContextName->setStyleSheet("color: #ff0000");
        this->ui->lineEditContextName->setStyleSheet("border-color: #ff0000");
    }
    else
    {
        this->ui->lineEditContextName->setStyleSheet("");
        this->ui->labelContextName->setStyleSheet("");
    }
}
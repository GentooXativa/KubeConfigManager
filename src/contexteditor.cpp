#include <QCompleter>
#include <QMessageBox>

#include "kubeparser.h"
#include "contexteditor.h"
#include "ui_contexteditor.h"

ContextEditor::ContextEditor(KubeContext *context, KubeConfig *kConfig, bool isNew, QWidget *parent) : QDialog(parent),
                                                                                                       ui(new Ui::ContextEditor)
{
    this->context = context;
    this->config = kConfig;
    this->isNewContext = isNew;

    ui->setupUi(this);

    this->setIconsAndActions();

    KubeConfigUtils utils(this->config, this);

    ui->lineEditContextName->setText(this->context->name);

    if (!this->context->clusterNamespace.isEmpty())
    {
        ui->lineEditNamespace->setText(this->context->clusterNamespace);
    }

    QStringList clusterList = utils.getClustersStringList();
    QStringListModel *clusterModel = new QStringListModel(clusterList);
    ui->comboBoxClusters->setModel(clusterModel);

    QCompleter *clusterCompleter = new QCompleter(clusterList, this);
    ui->comboBoxClusters->setCompleter(clusterCompleter);

    QStringList userList = utils.getUsersStringList();
    QStringListModel *userModel = new QStringListModel(userList);
    ui->comboBoxUsers->setModel(userModel);

    QCompleter *userCompleter = new QCompleter(userList, this);
    ui->comboBoxUsers->setCompleter(userCompleter);

    if (ui->comboBoxClusters->findText(this->context->cluster->name))
        ui->comboBoxClusters->setCurrentIndex(ui->comboBoxClusters->findText(this->context->cluster->name));

    if (ui->comboBoxUsers->findText(this->context->user->name))
        ui->comboBoxUsers->setCurrentIndex(ui->comboBoxUsers->findText(this->context->user->name));
}

ContextEditor::~ContextEditor()
{
}

void ContextEditor::onPushButtonSaveClicked()
{
    KubeConfigUtils utils(this->config, this);

    if (utils.contextExist(ui->lineEditContextName->text()) && this->isNewContext)
    {
        QMessageBox::warning(this, tr("Context already exists"), tr("A context with the same name already exists, please use another name."));
        return;
    }

    if (!utils.userExist(ui->comboBoxUsers->currentText()))
    {
        QMessageBox::warning(this, tr("User entry not found"), tr("User entry not found, please select one from the dropdown list."));
        return;
    }

    if (!utils.clusterExist(ui->comboBoxClusters->currentText()))
    {
        QMessageBox::warning(this, tr("Cluster entry not found"), tr("Cluster entry not found, please select one from the dropdown list."));
        return;
    }

    KubeContext editedContext;

    editedContext.name = ui->lineEditContextName->text();
    editedContext.clusterNamespace = ui->lineEditNamespace->text();

    editedContext.cluster = utils.getClusterByName(ui->comboBoxClusters->currentText());
    editedContext.user = utils.getUserByName(ui->comboBoxUsers->currentText());

    emit contextSaved(&editedContext);
    emit accepted();
    this->close();
}

void ContextEditor::onPushButtonCancelClicked()
{
    emit rejected();
    this->close();
}

void ContextEditor::checkContextName()
{
    if (ui->lineEditContextName->text().isEmpty())
    {

        ui->labelContextName->setStyleSheet("color: #ff0000");
        ui->lineEditContextName->setStyleSheet("border-color: #ff0000");
    }
    else
    {
        ui->lineEditContextName->setStyleSheet("");
        ui->labelContextName->setStyleSheet("");
    }
}

void ContextEditor::setIconsAndActions()
{
    const QIcon iconSave = QIcon::fromTheme("document-save", QIcon(":/icons/document-save"));
    const QIcon iconCancel = QIcon::fromTheme("process-stop", QIcon(":/icons/process-stop"));

    ui->pushButtonCancel->setIcon(iconCancel);
    ui->pushButtonSave->setIcon(iconSave);

    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &ContextEditor::onPushButtonCancelClicked);
    connect(ui->pushButtonSave, &QPushButton::clicked, this, &ContextEditor::onPushButtonSaveClicked);
    connect(ui->lineEditContextName, &QLineEdit::textChanged, this, &ContextEditor::checkContextName);
}

#include "clustereditor.h"
#include "ui_clustereditor.h"

ClusterEditor::ClusterEditor(KubeConfig *kubeconfig, QWidget *parent) : QDialog(parent),
                                                                        ui(new Ui::ClusterEditor)
{
    ui->setupUi(this);
    this->kubeconfig = kubeconfig;
    KubeConfigUtils kUtils(kubeconfig);

    this->clustersModel = new QStringListModel(kUtils.getClustersStringList());

    this->ui->listViewClusterList->setModel(this->clustersModel);
    this->ui->toolButtonRemoveCluster->setEnabled(false);

    connect(ui->listViewClusterList, &QListView::activated, this, &ClusterEditor::onClusterSelected);
}

ClusterEditor::~ClusterEditor()
{
    delete ui;
}

void ClusterEditor::onClusterSelected(const QModelIndex &index)
{
    KubeConfigUtils kUtils(kubeconfig);
    this->selectedCluster = kUtils.getClusterByName(index.data().toString());
    qDebug() << "Selected cluster: " << this->selectedCluster->name;

    this->fillUiWithClusterInfo(this->selectedCluster);
    this->ui->toolButtonRemoveCluster->setEnabled(true);
}

void ClusterEditor::on_pushButtonSaveCluster_clicked()
{
    KubeCluster editedCluster;
    editedCluster.name = this->ui->lineEditName->text();
    editedCluster.server = this->ui->lineEditServerUrl->text();
    editedCluster.disableCompression = this->ui->checkBoxDisableCompression->isChecked();
    editedCluster.insecureSkipTlsVerify = this->ui->checkBoxInsecureSkipTlsVerify->isChecked();
}

void ClusterEditor::on_pushButtonCloneCluster_clicked()
{
}

void ClusterEditor::on_pushButtonRevertCluster_clicked()
{
}

void ClusterEditor::on_toolButtonAddCluster_triggered(QAction *arg1)
{
}

void ClusterEditor::on_toolButtonRemoveCluster_triggered(QAction *arg1)
{
}

void ClusterEditor::fillUiWithClusterInfo(KubeCluster *cluster)
{
    this->ui->lineEditName->setText(this->selectedCluster->name);
    this->ui->lineEditServerUrl->setText(this->selectedCluster->server);

    this->ui->checkBoxInsecureSkipTlsVerify->setChecked(this->selectedCluster->insecureSkipTlsVerify);
    this->ui->checkBoxDisableCompression->setChecked(this->selectedCluster->disableCompression);

    if (!this->selectedCluster->certificateAuthorityData.isEmpty())
    {
        this->ui->lineEditCaData->setText(this->selectedCluster->certificateAuthorityData);
    }
    else
    {
        this->ui->lineEditCaData->clear();
    }

    if (!this->selectedCluster->certificateAuthority.isEmpty())
    {
        this->ui->lineEditCaFile->setText(this->selectedCluster->certificateAuthority);
    }
    else
    {
        this->ui->lineEditCaFile->clear();
    }

    if (!this->selectedCluster->proxyUrl.isEmpty())
    {
        this->ui->lineEditProxyUrl->setText(this->selectedCluster->proxyUrl);
    }
    else
    {
        this->ui->lineEditProxyUrl->clear();
    }

    if (!this->selectedCluster->tlsServerName.isEmpty())
    {
        this->ui->lineEditTlsName->setText(this->selectedCluster->tlsServerName);
    }
    else
    {
        this->ui->lineEditTlsName->clear();
    }
}

void ClusterEditor::on_pushButtonCaFileSearch_clicked()
{
    QString directory;
    if (this->selectedCluster->certificateAuthority.isEmpty())
    {
        directory = QDir::homePath();
    }
    else
    {
        directory = this->selectedCluster->certificateAuthority;
    }

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Certificate Authority key file"), directory, tr("Key Files (*.key *.p12 *.crt)"));

    this->ui->lineEditCaFile->setText(fileName);

    emit clusterInfoHasBeenEdited();
}

void ClusterEditor::updateUI()
{
}

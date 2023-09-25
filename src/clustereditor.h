#ifndef CLUSTEREDITOR_H
#define CLUSTEREDITOR_H

#include <QDialog>
#include <QStringListModel>
#include <QFileDialog>
#include <QAction>

#include "KubeConfManager.h"
#include "kubeconfigutils.h"

namespace Ui
{
    class ClusterEditor;
}

class ClusterEditor : public QDialog
{
    Q_OBJECT

public:
    explicit ClusterEditor(KubeConfig *kubeconfig, QWidget *parent = nullptr);
    ~ClusterEditor();

private slots:
    // legacy
    void on_pushButtonSaveCluster_clicked();
    void on_pushButtonCloneCluster_clicked();
    void on_pushButtonRevertCluster_clicked();
    void on_toolButtonAddCluster_triggered(QAction *arg1);
    void on_toolButtonRemoveCluster_triggered(QAction *arg1);
    void fillUiWithClusterInfo(KubeCluster *cluster);
    void on_pushButtonCaFileSearch_clicked();

    // new
    void onClusterSelected(const QModelIndex &index);

signals:
    void clusterInfoHasBeenEdited();

private:
    Ui::ClusterEditor *ui;

    KubeConfig *kubeconfig;
    KubeCluster *selectedCluster;

    QStringListModel *clustersModel;

    void updateUI();
};

#endif // CLUSTEREDITOR_H

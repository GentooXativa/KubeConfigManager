#ifndef CLUSTEREDITOR_H
#define CLUSTEREDITOR_H

#include <QWidget>
#include <QStringListModel>
#include <QFileDialog>

#include "KubeConfManager.h"
#include "kubeconfigutils.h"

namespace Ui
{
    class ClusterEditor;
}

class ClusterEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ClusterEditor(KubeConfig *kubeconfig, QWidget *parent = nullptr);
    ~ClusterEditor();

private slots:
    void on_listViewClusterList_activated(const QModelIndex &index);
    void on_pushButtonSaveCluster_clicked();
    void on_pushButtonCloneCluster_clicked();
    void on_pushButtonRevertCluster_clicked();
    void on_toolButtonAddCluster_triggered(QAction *arg1);
    void on_toolButtonRemoveCluster_triggered(QAction *arg1);
    void fillUiWithClusterInfo(KubeCluster *cluster);
    void on_pushButtonCaFileSearch_clicked();

signals:
    void clusterInfoHasBeenEdited();

private:
    Ui::ClusterEditor *ui;

    KubeConfig *kubeconfig;
    KubeCluster *selectedCluster;

    QStringListModel *clustersModel;
};

#endif // CLUSTEREDITOR_H

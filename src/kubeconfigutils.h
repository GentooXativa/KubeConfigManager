#ifndef KUBECONFIGUTILS_H
#define KUBECONFIGUTILS_H

#include <QObject>
#include <QDebug>

#include "kubeconfig.h"
#include "KubeConfManager.h"

class KubeConfigUtils : public QObject
{
    Q_OBJECT
public:
    KubeConfigUtils(){};
    explicit KubeConfigUtils(KubeConfig *kConfig, QObject *parent = nullptr);

    QString getCurrentContext();

    KubeContext *getContextByName(QString name);
    KubeCluster *getClusterByName(QString name);
    KubeUser *getUserByName(QString name);

    bool contextExist(const QString name);
    bool clusterExist(const QString name);
    bool userExist(const QString name);

    QStringList getClustersStringList();
    QStringList getContextsStringList();
    QStringList getUsersStringList();

private:
    KubeConfig *kubeConfig;
};

#endif // KUBECONFIGUTILS_H

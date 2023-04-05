#include "kubeconfigutils.h"

KubeConfigUtils::KubeConfigUtils(KubeConfig *kConfig, QObject *parent) : QObject(parent)
{
    this->kubeConfig = kConfig;
}

QString KubeConfigUtils::getCurrentContext()
{
    return this->kubeConfig->currentContext;
}

KubeContext *KubeConfigUtils::getContextByName(QString name)
{
    for (QList<KubeContext>::iterator it = this->kubeConfig->contexts->begin(); it != this->kubeConfig->contexts->end(); ++it)
    {
        KubeContext current = *it;
        if (name == current.name)
        {
            return new KubeContext(current);
        }
    }

    return NULL;
}

KubeUser *KubeConfigUtils::getUserByName(QString name)
{
    for (QList<KubeUser>::iterator it = this->kubeConfig->users->begin(); it != this->kubeConfig->users->end(); ++it)
    {
        KubeUser current = *it;
        if (name == current.name)
        {
            return new KubeUser(current);
        }
    }

    return NULL;
}

KubeCluster *KubeConfigUtils::getClusterByName(QString name)
{
    for (QList<KubeCluster>::iterator it = this->kubeConfig->clusters->begin(); it != this->kubeConfig->clusters->end(); ++it)
    {
        KubeCluster current = *it;
        if (name == current.name)
        {
            return new KubeCluster(current);
        }
    }

    return NULL;
}
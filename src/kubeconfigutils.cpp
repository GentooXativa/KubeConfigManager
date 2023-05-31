#include "kubeconfigutils.h"
#include "kubeparser.h"

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
    auto it = std::find_if(this->kubeConfig->contexts->begin(), this->kubeConfig->contexts->end(),
                           [name](const KubeContext &context)
                           { return context.name == name; });

    return (it != this->kubeConfig->contexts->end()) ? new KubeContext(*it) : new KubeContext();
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

    return new KubeUser();
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

    return new KubeCluster();
}

QStringList KubeConfigUtils::getClustersStringList()
{
    QStringList list;
    for (const auto &cluster : *this->kubeConfig->clusters)
    {
        list.append(cluster.name);
    }

    return list;
}

QStringList KubeConfigUtils::getContextsStringList()
{
    QStringList list;
    for (const auto &context : *this->kubeConfig->contexts)
    {
        list.append(context.name);
    }

    return list;
}

QStringList KubeConfigUtils::getUsersStringList()
{
    QStringList list;
    for (const auto &user : *this->kubeConfig->users)
    {
        list.append(user.name);
    }

    return list;
}

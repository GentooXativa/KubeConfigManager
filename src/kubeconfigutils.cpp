#include "kubeconfigutils.h"

KubeConfigUtils::KubeConfigUtils(KubeConfig *kConfig, QObject *parent) : QObject(parent)
{
    this->kubeConfig = kConfig;
}

QString KubeConfigUtils::getCurrentContext()
{
    return this->kubeConfig->currentContext;
}
KubeContext KubeConfigUtils::getContextByName(QString name)
{
    for (QList<KubeContext>::iterator it = this->kubeConfig->contexts->begin(); it != this->kubeConfig->contexts->end(); ++it)
    {
        KubeContext current = *it;
        if (name == current.name)
        {
            return current;
        }
    }
}
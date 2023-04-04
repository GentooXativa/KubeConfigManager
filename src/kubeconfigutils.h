#ifndef KUBECONFIGUTILS_H
#define KUBECONFIGUTILS_H

#include <QObject>
#include <QDebug>
#include "KubeConfManager.h"

class KubeConfigUtils : public QObject
{
    Q_OBJECT
public:
    explicit KubeConfigUtils(KubeConfig *kConfig, QObject *parent = nullptr);
    QString getCurrentContext();
    KubeContext getContextByName(QString name);

signals:

private:
    KubeConfig *kubeConfig;
};

#endif // KUBECONFIGUTILS_H

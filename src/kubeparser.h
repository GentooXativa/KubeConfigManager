#ifndef KUBEPARSER_H
#define KUBEPARSER_H

#include <QObject>
#include <QMap>

#include "kubeconfig.h"
#include "kubeconfigutils.h"
#include "KubeConfManager.h"

class KubeParser : public QObject
{
    Q_OBJECT
public:
    explicit KubeParser(QString path, QObject *parent = nullptr);
    ~KubeParser();

    KubeConfig *load();
    bool save();

    KubeConfig *getKubeConfig();

    static QString dumpConfig(KubeConfig *config);
    static KubeConfig *createEmptyConfig();

signals:
    void errorLoadingFile(QString message);
    void clustersLoaded(QStringList clusters);
    void contextsLoaded(QStringList contexts);
    void usersLoaded(QStringList users);

    void contextListLoaded(KubeContextMap *);
    void kubeConfigLoaded(KubeConfig *);

private:
    QString path;

    KubeContextMap *contexts;
    KubeClusterMap *clusters;
    KubeUserMap *users;

    KubeConfig *kubeConfig;
};

#endif // KUBEPARSER_H

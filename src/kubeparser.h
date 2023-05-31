#ifndef KUBEPARSER_H
#define KUBEPARSER_H

#include <QObject>
#include <QMap>

#include "kubeconfigutils.h"
#include "KubeConfManager.h"

class KubeParser : public QObject
{
    Q_OBJECT
public:
    explicit KubeParser(QString path, QObject *parent = nullptr);
    ~KubeParser();

    void load();
    bool save();

    KubeConfig *getKubeConfig();

    static QString dumpConfig(KubeConfig *config);
    static KubeConfig *createEmptyConfig();
signals:
    void errorLoadingFile(QString message);
    void clustersLoaded(QStringList clusters);
    void contextsLoaded(QStringList contexts);
    void usersLoaded(QStringList users);

    void contextListLoaded(KubeContextList *);
    void kubeConfigLoaded(KubeConfig *);

private:
    QString path;

    KubeContextList *contexts;
    KubeClusterList *clusters;
    KubeUserList *users;

    KubeConfig *kubeConfig;
};

#endif // KUBEPARSER_H

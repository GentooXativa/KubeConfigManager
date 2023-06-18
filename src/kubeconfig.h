#ifndef KUBECONFIG_H
#define KUBECONFIG_H

#include <QObject>

#include "KubeConfManager.h"

class KubeConfig : public QObject
{
    Q_OBJECT
public:
    explicit KubeConfig(KubeConfig *prev) : QObject(prev), m_clusters(prev->clusters()), m_users(prev->users()),
                                            m_contexts(prev->contexts()), m_currentContext(prev->currentContext()),
                                            m_originalFilePath(prev->originalFilePath()) {}
    explicit KubeConfig(QObject *parent = nullptr);
    ~KubeConfig();

    KubeClusterMap *clusters();
    KubeUserMap *users();
    KubeContextMap *contexts();

    QStringList clustersNames();
    QStringList usersNames();
    QStringList contextsNames();

    const QString &currentContext() const;
    const QString &originalFilePath() const;

    void setOriginalFilePath(const QString &path);
    void setCurrentContext(const QString &contextName);

    void setClusters(KubeClusterMap *list);
    void setContexts(KubeContextMap *list);
    void setUsers(KubeUserMap *list);

    void updateContext(const QString &name, KubeContext *newContext);
    static void debug(KubeConfig *config);

    bool save();
    bool saveAs(const QString &path);

private:
    KubeClusterMap *m_clusters;
    KubeUserMap *m_users;
    KubeContextMap *m_contexts;
    QString m_currentContext;
    QVariant *m_preferences;
    QString m_originalFilePath;
};

#endif // KUBECONFIG_H

#ifndef KUBECONFMANAGER_H
#define KUBECONFMANAGER_H

#define APP_NAME "KubeConfManager"
#define APP_VERSION "0.1"
#define APP_BUG_CONTACT "jvginer@gentooxativa.com"
#define APP_REPOSITORY "https://github.com/GentooXativa/KubeConfigManager"
#define APP_ISSUES_URL "https://github.com/GentooXativa/KubeConfigManager/issues"

#include <QString>
#include <QByteArray>
#include <QMap>
#include <QVariantMap>
#include <QDateTime>

struct KubeCluster
{
    QString name;

    QString server;
    QString tlsServerName;
    bool insecureSkipTlsVerify;
    QString certificateAuthority;
    QString certificateAuthorityData;
    QString proxyUrl;
    bool disableCompression;
    QMap<QString, QString> extensions;

    QString caCertificate;
};

struct KubeAuthExec
{
    QString apiVersion;
    QString args;
    QString command;
    QString env;
    QString installHint;
    QString interactiveMode;
    QString provideClusterInfo;
};

struct KubeAuthProvider
{
    QString name;
    QMap<QString, QString> config;
};

struct KubeUser
{
    QString name;
    KubeAuthExec exec;
    KubeAuthProvider provider;

    QString clientCertificateData;
    QString clientKeyData;
    QString token;
};

struct KubeContext
{
    QString name;
    KubeCluster *cluster;
    KubeUser *user;
    QString clusterNamespace;
    QMap<QString, QString> extensions;
};

typedef QList<KubeContext> KubeContextList;
typedef QList<KubeCluster> KubeClusterList;
typedef QList<KubeUser> KubeUserList;

struct KubeConfig
{
    KubeClusterList *clusters;
    KubeUserList *users;
    KubeContextList *contexts;
    QString currentContext;
    QVariant *preferences;
    QString *originalFilePath;
};

#endif // KUBECONFMANAGER_H

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

struct KubeConfigExtension
{
    QString name;
    QString provider;
    QString version;
    QString lastUpdate;
};

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
    QList<KubeConfigExtension> extensions;

    QString caCertificate;
};

struct KubeAuthExec
{
    QString apiVersion;
    QStringList args;
    QString command;
    QMap<QString, QString> env;
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

    QString clientCertificate;
    QString clientCertificateData;
    QString clientKey;
    QString clientKeyData;
    QString token;
    QString tokenFile;
    QString as;
    QString asUid;
    QStringList asGroups;
    QMap<QString, QString> asUserExtra;
    QString username;
    QString password;
};

struct KubeContext
{
    QString name;
    KubeCluster *cluster;
    KubeUser *user;
    QString clusterNamespace;
    QList<KubeConfigExtension> extensions;
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

#define kTrace qDebug() << ">> [TRACE]" << __PRETTY_FUNCTION__

#endif // KUBECONFMANAGER_H

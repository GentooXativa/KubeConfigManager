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

typedef QMap<QString, KubeContext> KubeContextMap;
typedef QMap<QString, KubeCluster> KubeClusterMap;
typedef QMap<QString, KubeUser> KubeUserMap;

#ifdef KCM_TRACE_FUNCTIONS
#define kTrace qDebug() << "[\x1b[33;1m*\x1b[0m]" << __PRETTY_FUNCTION__
#else
#define kTrace void();
#endif

#endif // KUBECONFMANAGER_H

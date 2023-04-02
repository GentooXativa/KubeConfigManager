#ifndef KUBECONFMANAGER_H
#define KUBECONFMANAGER_H

#define APP_NAME            "KubeConfManager"
#define APP_VERSION         "0.1"
#define APP_BUG_CONTACT     "jvginer@gentooxativa.com"
#define APP_REPOSITORY      "https://github.com/GentooXativa/KubeConfigManager"
#define APP_ISSUES_URL      "https://github.com/GentooXativa/KubeConfigManager/issues"

#include <QString>
#include <QMap>

struct KubeContext
{
    QString name;
    QMap<QString, QString> context;
};

typedef QList<KubeContext> KubeContextList;

#endif // KUBECONFMANAGER_H

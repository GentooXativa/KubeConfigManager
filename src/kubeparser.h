#ifndef KUBEPARSER_H
#define KUBEPARSER_H

#include <QObject>
#include <QMap>

#include "KubeConfManager.h"

class KubeParser : public QObject
{
    Q_OBJECT
public:
    explicit KubeParser(QString path, QObject *parent = nullptr);

    bool load();
    bool save();

    KubeContext *getContextByName(const QString name);

signals:
    void errorLoadingFile(QString message);
    void clustersLoaded(QStringList clusters);
    void contextsLoaded(QStringList contexts);
    void usersLoaded(QStringList users);

    void contextListLoaded(KubeContextList*);
private:
    QString path;

    KubeContextList *contexts;
};

#endif // KUBEPARSER_H

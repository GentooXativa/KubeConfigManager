#ifndef KUBEPARSER_H
#define KUBEPARSER_H

#include <QObject>

class KubeParser : public QObject
{
    Q_OBJECT
public:
    explicit KubeParser( QString path, QObject *parent = nullptr);

    bool    load();
    bool    save();

signals:
    void    errorLoadingFile(QString message);
    void    clustersLoaded(QStringList clusters);
    void    contextsLoaded(QStringList contexts);
    void    usersLoaded(QStringList users);

private:
    QString     path;
};

#endif // KUBEPARSER_H
#include "kubeparser.h"

#include <QDebug>
#include <QFile>
#include <QMap>

#include <yaml-cpp/yaml.h>

KubeParser::KubeParser(QString path, QObject *parent) : QObject(parent)
{
    this->path = path;
    qDebug() << "Initializing parser for file:" << path;
}

bool KubeParser::load()
{
    QFile file(path);

    QStringList clusters;
    QStringList contexts;
    QStringList users;

    this->contexts = new KubeContextList();

    if (!file.exists())
    {
        emit errorLoadingFile("File not found or cannot be accessed!");
        return false;
    }

    YAML::Node config = YAML::LoadFile(path.toStdString());

    if (config["clusters"])
    {
        for (const auto &cluster : config["clusters"])
        {
            qDebug() << "Cluster found:" << cluster["name"].as<std::string>().c_str();
            clusters.append(QString::fromStdString(cluster["name"].as<std::string>()));
        }
        emit clustersLoaded(clusters);
    }

    if (config["contexts"])
    {
        for (const auto &context : config["contexts"])
        {
            qDebug() << "Context found:" << context["name"].as<std::string>().c_str();
            contexts.append(QString::fromStdString(context["name"].as<std::string>()));

            KubeContext ctx;
            ctx.name = QString::fromStdString(context["name"].as<std::string>().c_str());

            QMap<QString, QString> ctxDetails;
            for (YAML::const_iterator it = context.begin(); it != context.end(); ++it)
            {
                YAML::Node fnode = it->first;
                YAML::Node snode = it->second;

                if (snode.IsScalar())
                {
                    qDebug() << "\tScalar type |" << snode.as<std::string>().c_str();
                }
                else if (snode.IsMap())
                {
                    QString clusterName(snode["cluster"].as<std::string>().c_str());
                    QString userName(snode["user"].as<std::string>().c_str());

                    ctxDetails.insert(QString("cluster"), clusterName);
                    ctxDetails.insert(QString("user"), userName);

                    qDebug() << "\tMap type | Cluster:" << clusterName << " | User:" << userName;

                    ctx.context = ctxDetails;
                }
                else
                {
                    qDebug() << "\tUnhandled type";
                }
            }
            this->contexts->append(ctx);
        }

        emit contextsLoaded(contexts);
        emit contextListLoaded(this->contexts);
    }

    if (config["users"])
    {
        for (const auto &user : config["users"])
        {
            qDebug() << "User found:" << user["name"].as<std::string>().c_str();
            users.append(QString::fromStdString(user["name"].as<std::string>()));
        }
        emit usersLoaded(users);
    }

    return false;
}

bool KubeParser::save()
{
    return false;
}

KubeContext * KubeParser::getContextByName(const QString name) {

}

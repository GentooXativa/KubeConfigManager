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

void KubeParser::load()
{
    QFile file(path);

    QStringList clusters;
    QStringList contexts;
    QStringList users;

    this->kubeConfig = new KubeConfig();
    this->kubeConfig->originalFilePath = &path;

    this->contexts = new KubeContextList();
    this->clusters = new KubeClusterList();
    this->users = new KubeUserList();

    if (!file.exists())
    {
        emit errorLoadingFile("File not found or cannot be accessed!");
        return;
    }

    YAML::Node config = YAML::LoadFile(path.toStdString());

    if (config["current-context"])
    {
        QString currentContext = QString::fromStdString(config["current-context"].as<std::string>());
        qDebug() << "This config file has this context as current:" << currentContext;
        this->kubeConfig->currentContext = currentContext;
    }

    if (config["clusters"])
    {
        for (const auto &cluster : config["clusters"])
        {
            qDebug() << "Cluster found  :" << cluster["name"].as<std::string>().c_str();
            clusters.append(QString::fromStdString(cluster["name"].as<std::string>()));

            KubeCluster clusterObj;
            clusterObj.name = QString::fromStdString(cluster["name"].as<std::string>().c_str());

            clusterObj.server = QString::fromStdString(cluster["cluster"]["server"].as<std::string>());
            qDebug() << "Server         :" << clusterObj.server;

            if (cluster["cluster"]["certificate-authority-data"])
            {
                clusterObj.caCertificate = QString::fromStdString(cluster["cluster"]["certificate-authority-data"].as<std::string>());
            }

            this->clusters->append(clusterObj);
        }
        this->kubeConfig->clusters = this->clusters;
        qDebug() << "Cluster definitions lodaded:" << this->clusters->size();
    }

    if (config["users"])
    {
        for (const auto &user : config["users"])
        {
            KubeUser userObj;
            qDebug() << "User found:" << user["name"].as<std::string>().c_str();
            userObj.name = QString::fromStdString(user["name"].as<std::string>());
            users.append(userObj.name);

            if (!user["user"])
            {
                qErrnoWarning("Cannot locate 'user' keyword at config file");
                emit errorLoadingFile(QString("File does not look compatible, please check documentation or raise a ticket at %1").arg(APP_ISSUES_URL));
                throw;
            }

            /**
             * Legacy K8S user
             */
            if (user["user"]["client-certificate-data"])
            {
                userObj.clientCertificateData = QString::fromStdString(user["user"]["client-certificate-data"].as<std::string>());
            }
            if (user["user"]["client-key-data"])
            {
                userObj.clientKeyData = QString::fromStdString(user["user"]["client-key-data"].as<std::string>());
            }
            if (user["user"]["token"])
            {
                userObj.token = QString::fromStdString(user["user"]["token"].as<std::string>());
            }

            if (user["user"]["exec"])
            {
                YAML::Node execData = user["user"]["exec"];
                userObj.exec.apiVersion = QString::fromStdString(execData["apiVersion"].as<std::string>());
                userObj.exec.command = QString::fromStdString(execData["command"].as<std::string>());

                qDebug() << "User auth exec plugin:" << userObj.exec.command;
                userObj.exec.env = QString::fromStdString(execData["env"].as<std::string>());
                userObj.exec.args = QString::fromStdString(execData["args"].as<std::string>());
                userObj.exec.installHint = QString::fromStdString(execData["installHint"].as<std::string>());
                userObj.exec.interactiveMode = QString::fromStdString(execData["interactiveMode"].as<std::string>());
                userObj.exec.provideClusterInfo = QString::fromStdString(execData["provideClusterInfo"].as<std::string>());
            }

            if (false)
            {
                for (YAML::const_iterator it = user.begin(); it != user.end(); ++it)
                {
                    qDebug() << "\t" << QString::fromStdString(it->first.as<std::string>()) << " | Size:" << it->second.size();
                    if (it->second.IsMap())
                    {
                        for (auto child = it->second.begin(); child != it->second.end(); child++)
                        {
                            qDebug() << "\t\t" << QString::fromStdString(child->first.as<std::string>()) << " | Size:" << child->second.size();
                            if (child->second.IsMap())
                            {
                                for (auto data = child->second.begin(); data != child->second.end(); data++)
                                {
                                    qDebug() << "\t\t\t" << QString::fromStdString(data->first.as<std::string>()) << " | Size:" << data->second.size();
                                }
                            }
                        }
                    }
                }
            }
            this->users->append(userObj);
        }
        this->kubeConfig->users = this->users;
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

        this->kubeConfig->contexts = this->contexts;
    }

    emit kubeConfigLoaded(this->kubeConfig);
}

bool KubeParser::save()
{
    return false;
}

KubeConfig *KubeParser::getKubeConfig()
{
    return this->kubeConfig;
}
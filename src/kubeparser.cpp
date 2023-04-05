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

            // Required properties
            clusterObj.name = QString::fromStdString(cluster["name"].as<std::string>().c_str());
            clusterObj.server = QString::fromStdString(cluster["cluster"]["server"].as<std::string>());

            // Optional properties
            if (cluster["cluster"]["certificate-authority-data"])
            {
                clusterObj.certificateAuthorityData = QString::fromStdString(cluster["cluster"]["certificate-authority-data"].as<std::string>());
            }

            if (cluster["cluster"]["certificate-authority"])
            {
                clusterObj.certificateAuthority = QString::fromStdString(cluster["cluster"]["certificate-authority"].as<std::string>());
            }

            if (cluster["cluster"]["tls-server-name"])
            {
                clusterObj.tlsServerName = QString::fromStdString(cluster["cluster"]["tls-server-name"].as<std::string>());
            }

            if (cluster["cluster"]["insecure-skip-tls-verify"])
            {
                clusterObj.insecureSkipTlsVerify = cluster["cluster"]["insecure-skip-tls-verify"].as<bool>();
            }

            if (cluster["cluster"]["disable-compression"])
            {
                clusterObj.disableCompression = cluster["cluster"]["disable-compression"].as<bool>();
            }

            if (cluster["cluster"]["proxy-url"])
            {
                clusterObj.proxyUrl = QString::fromStdString(cluster["cluster"]["proxy-url"].as<std::string>());
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
        KubeConfigUtils kUtils(this->kubeConfig);

        for (const auto &context : config["contexts"])
        {
            qDebug() << "Context found:" << context["name"].as<std::string>().c_str();
            contexts.append(QString::fromStdString(context["name"].as<std::string>()));

            KubeContext contextObj;
            contextObj.name = QString::fromStdString(context["name"].as<std::string>().c_str());

            QString ctxUser = QString::fromStdString(context["context"]["user"].as<std::string>().c_str());
            QString ctxCluster = QString::fromStdString(context["context"]["cluster"].as<std::string>().c_str());

            KubeUser *ctxUserObj = kUtils.getUserByName(ctxUser);
            KubeCluster *ctxClusterObj = kUtils.getClusterByName(ctxCluster);

            if (!ctxUserObj)
            {
                emit errorLoadingFile(QString("Error loading context %1, user %2 cannot be found!").arg(contextObj.name, ctxUser));
                return;
            }

            if (!ctxClusterObj)
            {
                emit errorLoadingFile(QString("Error loading context %1, cluster %2 cannot be found!").arg(contextObj.name, ctxCluster));
                return;
            }

            contextObj.cluster = ctxClusterObj;
            contextObj.user = ctxUserObj;

            // optional properties
            if (context["context"]["namespace"])
            {
            }

            this->contexts->append(contextObj);
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
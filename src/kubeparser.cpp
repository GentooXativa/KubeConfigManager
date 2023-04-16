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
            clusterObj.insecureSkipTlsVerify = false;
            clusterObj.disableCompression = false;

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

            if (user["user"]["client-certificate-data"])
            {
                userObj.clientCertificateData = QString::fromStdString(user["user"]["client-certificate-data"].as<std::string>());
            }

            if (user["user"]["client-certificate"])
            {
                userObj.clientCertificate = QString::fromStdString(user["user"]["client-certificate"].as<std::string>());
            }

            if (user["user"]["client-key-data"])
            {
                userObj.clientKeyData = QString::fromStdString(user["user"]["client-key-data"].as<std::string>());
            }

            if (user["user"]["client-key"])
            {
                userObj.clientKey = QString::fromStdString(user["user"]["client-key"].as<std::string>());
            }

            if (user["user"]["token"])
            {
                userObj.token = QString::fromStdString(user["user"]["token"].as<std::string>());
            }

            if (user["user"]["as-uid"])
            {
                userObj.asUid = QString::fromStdString(user["user"]["as-uid"].as<std::string>());
            }

            if (user["user"]["as"])
            {
                userObj.as = QString::fromStdString(user["user"]["as"].as<std::string>());
            }

            if (user["user"]["tokenFile"])
            {
                userObj.tokenFile = QString::fromStdString(user["user"]["tokenFile"].as<std::string>());
            }

            if (user["user"]["username"])
            {
                userObj.username = QString::fromStdString(user["user"]["username"].as<std::string>());
            }

            if (user["user"]["password"])
            {
                userObj.password = QString::fromStdString(user["user"]["password"].as<std::string>());
            }

            if (user["user"]["exec"])
            {
                YAML::Node execData = user["user"]["exec"];
                // required properties
                userObj.exec.apiVersion = QString::fromStdString(execData["apiVersion"].as<std::string>());
                userObj.exec.command = QString::fromStdString(execData["command"].as<std::string>());
                userObj.exec.installHint = QString::fromStdString(execData["installHint"].as<std::string>());
                userObj.exec.interactiveMode = QString::fromStdString(execData["interactiveMode"].as<std::string>());
                userObj.exec.provideClusterInfo = QString::fromStdString(execData["provideClusterInfo"].as<std::string>());

                // optional properties
                if (execData["env"] && execData["env"].Type() != YAML::NodeType::Null)
                {
                    if (execData["env"].IsSequence())
                    {
                        QMap<QString, QString> envVars;
                        for (YAML::const_iterator it = execData["env"].begin(); it != execData["env"].end(); ++it)
                        {
                            YAML::Node envVar = *it;
                            QString envVarName = QString::fromStdString(envVar["name"].as<std::string>());
                            QString envVarValue = QString::fromStdString(envVar["value"].as<std::string>());
                            envVars.insert(envVarName, envVarValue);
                        }

                        userObj.exec.env = envVars;
                    }
                }

                if (execData["args"] && execData["args"].Type() != YAML::NodeType::Null)
                {
                    if (execData["args"].IsSequence())
                    {
                        QStringList args;
                        for (YAML::const_iterator it = execData["args"].begin(); it != execData["args"].end(); ++it)
                        {
                            args.append(QString::fromStdString(it->as<std::string>()));
                        }

                        userObj.exec.args = args;
                    }
                }
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
                contextObj.clusterNamespace = QString::fromStdString(context["context"]["namespace"].as<std::string>().c_str());
            }

            if (context["context"]["extensions"])
            {
                YAML::Node extensionsNode = context["context"]["extensions"];
                QList<KubeConfigExtension> extensions;
                for (YAML::const_iterator it = extensionsNode.begin(); it != extensionsNode.end(); ++it)
                {
                    YAML::Node extensionDetailNode = *it;
                    KubeConfigExtension extension;

                    extension.name = QString::fromStdString(extensionDetailNode["name"].as<std::string>());
                    extension.provider = QString::fromStdString(extensionDetailNode["extension"]["provider"].as<std::string>());
                    extension.version = QString::fromStdString(extensionDetailNode["extension"]["version"].as<std::string>());
                    extension.lastUpdate = QString::fromStdString(extensionDetailNode["extension"]["last-update"].as<std::string>());

                    extensions.append(extension);
                }
                contextObj.extensions = extensions;
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

QString KubeParser::dumpConfig(KubeConfig *config)
{
    YAML::Emitter emitter;

    emitter << YAML::BeginMap;
    // common fields
    emitter << YAML::Key << "apiVersion" << YAML::Value << "v1";
    emitter << YAML::Key << "kind" << YAML::Value << "Config";

    // start of clusters sequence
    emitter << YAML::Key << "clusters";
    emitter << YAML::BeginSeq;
    for (QList<KubeCluster>::iterator it = config->clusters->begin(); it != config->clusters->end(); ++it)
    {
        KubeCluster currentCluster = *it;
        emitter << YAML::BeginMap;
        // required fields
        emitter << YAML::Key << "name" << YAML::Value << currentCluster.name.toStdString();
        emitter << YAML::Key << "server" << YAML::Value << currentCluster.server.toStdString();

        // optional fields
        emitter << YAML::EndMap;
    }
    emitter << YAML::EndSeq;
    // end of clusters sequence

    // start of users sequence
    emitter << YAML::Key << "users";
    emitter << YAML::BeginSeq;
    for (QList<KubeUser>::iterator it = config->users->begin(); it != config->users->end(); ++it)
    {
        KubeUser currentuser = *it;
        emitter << YAML::BeginMap;
        // required fields
        emitter << YAML::Key << "name" << YAML::Value << currentuser.name.toStdString();

        // optional fields
        emitter << YAML::EndMap;
    }
    emitter << YAML::EndSeq;
    // end of users sequence

    // start of contexts sequence
    emitter << YAML::Key << "contexts";
    emitter << YAML::BeginSeq;
    for (QList<KubeContext>::iterator it = config->contexts->begin(); it != config->contexts->end(); ++it)
    {
        KubeContext currentcontext = *it;
        emitter << YAML::BeginMap;
        // required fields
        emitter << YAML::Key << "name" << YAML::Value << currentcontext.name.toStdString();
        emitter << YAML::Key << "user" << YAML::Value << currentcontext.user->name.toStdString();
        emitter << YAML::Key << "cluster" << YAML::Value << currentcontext.cluster->name.toStdString();

        // optional fields
        emitter << YAML::EndMap;
    }
    emitter << YAML::EndSeq;
    // end of contexts sequence

    emitter << YAML::EndMap;

    return QString::fromStdString(emitter.c_str());
}
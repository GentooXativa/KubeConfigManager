#include <QDir>
#include <QDebug>
#include <QSettings>
#include <QMessageBox>

#include "kubeconfig.h"

KubeConfig::KubeConfig(QObject *parent) : QObject(parent)
{
    kTrace;
    this->m_contexts = new KubeContextMap();
    this->m_clusters = new KubeClusterMap();
    this->m_users = new KubeUserMap();

    this->m_currentContext = "";
    this->m_preferences = NULL;
    this->m_originalFilePath = "";
}

KubeConfig::~KubeConfig()
{
    kTrace;
}

KubeClusterMap *KubeConfig::clusters()
{
    kTrace;
    return this->m_clusters;
}

KubeUserMap *KubeConfig::users()
{
    kTrace;
    return this->m_users;
}

KubeContextMap *KubeConfig::contexts()
{
    kTrace;
    return this->m_contexts;
}

QStringList KubeConfig::clustersNames()
{
    kTrace;
    QStringList clusterNames;
    auto keys = this->m_clusters->keys();
    for (auto key : keys)
        clusterNames.append(key.toUtf8());

    return clusterNames;
}

QStringList KubeConfig::usersNames()
{
    kTrace;
    QStringList userNames;
    auto keys = this->m_users->keys();
    for (auto key : keys)
        userNames.append(key.toUtf8());

    return userNames;
}

QStringList KubeConfig::contextsNames()
{
    kTrace;
    QStringList contextNames;
    auto keys = this->m_contexts->keys();
    for (auto key : keys)
        contextNames.append(key.toUtf8());

    return contextNames;
}

const QString &KubeConfig::currentContext() const
{
    kTrace;
    return this->m_currentContext;
}

const QString &KubeConfig::originalFilePath() const
{
    kTrace;
    return this->m_originalFilePath;
}

void KubeConfig::setOriginalFilePath(const QString &path)
{
    kTrace;
    this->m_originalFilePath = path;
}

void KubeConfig::setCurrentContext(const QString &contextName)
{
    kTrace;
    this->m_currentContext = contextName;
}

void KubeConfig::setClusters(KubeClusterMap *list)
{
    kTrace;
    qDebug() << "Setting another cluster list | Previous: " << this->m_clusters->size() << " | New: " << list->size();
    this->m_clusters = list;
}

void KubeConfig::setContexts(KubeContextMap *list)
{
    kTrace;
    qDebug() << "Setting another context list | Previous: " << this->m_contexts->size() << " | New: " << list->size();
    this->m_contexts = list;
}

void KubeConfig::setUsers(KubeUserMap *list)
{
    kTrace;
    qDebug() << "Setting another user list | Previous: " << this->m_users->size() << " | New: " << list->size();
    this->m_users = list;
}

void KubeConfig::updateContext(const QString &name, KubeContext *newContext)
{
    kTrace;
    qDebug() << "\tPrev context:" << name << "\tNew:" << newContext->name;
    KubeContext prev = this->contexts()->value(name);
    qDebug() << "\tPrev cluster:" << prev.cluster->name << "\tNew:" << newContext->cluster->name;
    qDebug() << "\tPrev user:" << prev.user->name << "\tNew:" << newContext->user->name;

    this->contexts()->remove(name);
    this->contexts()->insert(newContext->name, *newContext);
}

void KubeConfig::debug(KubeConfig *config)
{
#ifndef QT_DEBUG
    return;
#endif

    qDebug() << "\n           KubeConfig dump\n========================================";

    if (!config->originalFilePath().isEmpty())
        qDebug() << "File path:\x1b[32m" << config->originalFilePath() << "\x1b[0m\n";

    if (config->contexts()->size() == 0)
        qDebug() << "Contexts :\x1b[31m 0\x1b[0m";
    else
        qDebug() << "Contexts :\x1b[32m" << config->contexts()->size() << "\x1b[0m";

    if (config->users()->size() == 0)
        qDebug() << "Users    :\x1b[31m 0\x1b[0m";
    else
        qDebug() << "Users    :\x1b[32m" << config->users()->size() << "\x1b[0m";

    if (config->clusters()->size() == 0)
        qDebug() << "Clusters :\x1b[31m 0\x1b[0m";
    else
        qDebug() << "Clusters :\x1b[32m" << config->clusters()->size() << "\x1b[0m";

    qDebug() << "\nContexts\n========================================";
    for (const auto &context : *config->contexts())
    {
        qDebug() << "\t \x1b[32m"
                 << context.name << "\x1b[0m";
    }

    qDebug() << "\nClusters\n========================================";
    for (const auto &cluster : *config->clusters())
    {
        qDebug() << "\t \x1b[32m"
                 << cluster.name << "\x1b[0m";
    }

    qDebug() << "\nUsers\n========================================";
    for (const auto &user : *config->users())
    {
        qDebug() << "\t \x1b[32m"
                 << user.name << "\x1b[0m";
    }

    qDebug() << "\n";
}

bool KubeConfig::save()
{
    return this->saveAs(this->m_originalFilePath);
}

bool KubeConfig::saveAs(const QString &path)
{
    bool createBackup = false;
    int historySize = -1;
    QString backupPath;
    QDir backupDir;

    qDebug() << "[Save] Saving kubeconfig as:" << path;
    QSettings appSettings(QSettings::UserScope, "GentooXativa", "KubeConfManager", this);

    if (appSettings.contains("backup/configuration"))
    {
        createBackup = appSettings.value("backup/configuration").toBool();
        if (createBackup)
        {
            qDebug() << "[Save] Backup previous kubeconfig enabled";

            if (appSettings.contains("backup/history_size"))
                historySize = appSettings.value("backup/history_size").toInt();
            else
                historySize = 4;

            if (!appSettings.contains("paths/backup"))
            {
                QMessageBox::critical(nullptr, tr("Backup path not found"), tr("We dont know how, but backup is activated but you didnt set a directory!"));
                return false;
            }
            else
            {
                backupPath = appSettings.value("paths/backup").toString();
            }

            qDebug() << "[Save] Backup history size:" << historySize << " Path:" << backupPath;

            backupDir = QDir(backupPath);

            if (!backupDir.exists())
            {
                QMessageBox::critical(nullptr, tr("Backup directory not found"), QString(tr("Directory %1 does not exists to store backups.")).arg(backupPath));
                return false;
            }
        }

        if (historySize >= 1)
        {
            QFileInfo filename(path);
            QStringList prefix(QString("%1-*").arg(filename.baseName()));
            QFileInfoList files = backupDir.entryInfoList(prefix, QDir::Files, QDir::Time | QDir::Reversed);
            qDebug() << "[Save] Total files:" << files.size() << "Prefixes:" << prefix;

            int currentPos = 0;

            for (const auto &fileInfo : files)
            {
                if (++currentPos < historySize)
                {
                    qDebug() << "[Save] Keeping" << fileInfo.baseName();
                }
                else
                {
                    qDebug() << "[Save] Removing" << fileInfo.baseName();
                }
            }
        }
    }
    return false;
}

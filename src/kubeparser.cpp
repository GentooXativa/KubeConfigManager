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

bool KubeParser::load(){
    QFile file(path);

    QStringList clusters;
    QStringList contexts;
    QStringList users;

    if( !file.exists()){
        emit errorLoadingFile("File not found or cannot be accessed!");
        return false;
    }

    YAML::Node config = YAML::LoadFile(path.toStdString());

    if( config["clusters"]){
        for(const auto& cluster : config["clusters"]) {
            qDebug() << "Cluster found:" << cluster["name"].as<std::string>().c_str();
            clusters.append(QString::fromStdString(cluster["name"].as<std::string>()));
        }
        emit clustersLoaded(clusters);
    }

    if( config["contexts"]){
        for(const auto& context : config["contexts"]) {
            qDebug() << "Context found:" << context["name"].as<std::string>().c_str();
            contexts.append(QString::fromStdString(context["name"].as<std::string>()));
        }
        emit contextsLoaded(contexts);
    }

    if( config["users"]){
        for(const auto& user : config["users"]) {
            qDebug() << "User found:" << user["name"].as<std::string>().c_str();
            users.append(QString::fromStdString(user["name"].as<std::string>()));
        }
        emit usersLoaded(users);
    }

    return false;
}

bool KubeParser::save(){
    return false;
}

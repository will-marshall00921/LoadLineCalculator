// LoadLineCalculator -- Config.cpp
// Author: Will Marshall
// Created: 7/19/2026, 3:43:21 PM

#include "Config.hpp"
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QFileInfoList>
#include <QtWidgets/QApplication>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QJsonDocument>
#include <QtCore/QFile>
#include <QtCore/QByteArray>
#ifdef DEBUG_BUILD
#include <QtCore/QDebug>
#endif // DEBUG_BUILD

Config::Config(QObject* parent) 
  : QObject { parent } 
  {
  QString config_dir = QStandardPaths::writableLocation(
    QStandardPaths::StandardLocation::AppDataLocation
  );
  QString config_path = config_dir + "/" + "configuration.json";
  if (!QFileInfo::exists(config_path)) {
    QDir dir(config_dir);
    if (!dir.exists()) {
      dir.mkpath(config_dir);
    }
    #ifdef DEBUG_BUILD
    qDebug() << "Configuration path: " << config_path;
    #endif // DEBUG_BUILD
    Parameters default_parameters = createDefaultConfigFile(config_path);
    m_parameters = default_parameters;
  } else {
    readConfigFile(config_path);
  }
}

const Config::Parameters& Config::currentParameters() const noexcept {
  return m_parameters;
}

QString Config::tubePath(const QString& name) const {
  return m_tube_map.value(name, QString(""));
}

void Config::parseTubeDirectory() {
  m_tube_map.clear();
  QFileInfoList tube_files = QDir(m_parameters.tube_directory).entryInfoList(
    QDir::Filter::Readable | QDir::Filter::Files,
    QDir::SortFlag::NoSort
  );
  for (const QFileInfo& tube_file_info : tube_files) {
    m_tube_map.insert(tube_file_info.baseName(), tube_file_info.absoluteFilePath());
  }
  emit configMessage(QString("Found ") + QString::number(m_tube_map.size()) + QString(" tubes!"));
  emit tubeDirectoryParsed(m_tube_map.keys());
}

Config::Parameters Config::createDefaultConfigFile(const QString& path) {
  QString default_tube_directory = (
    QStandardPaths::writableLocation(
      QStandardPaths::StandardLocation::AppDataLocation
    ) + "/tubes"
  );
  QJsonObject config_top_object;
  config_top_object.insert("tube_directory", QJsonValue::fromVariant(default_tube_directory));
  QJsonDocument config_json(config_top_object);
  QByteArray json_data = config_json.toJson();
  QDir tube_dir(default_tube_directory);
  if (!tube_dir.exists()) {
    tube_dir.mkpath(default_tube_directory);
    #ifdef DEBUG_BUILD
    qDebug() << "Created directory: " << default_tube_directory;
    #endif // DEBUG_BUILD
  }
  QFile config_file(path);
  if (config_file.open(QIODevice::WriteOnly)) {
    config_file.write(json_data);
    config_file.close();
  }
  Parameters default_parameters;
  default_parameters.tube_directory = default_tube_directory;
  return default_parameters;
}

void Config::readConfigFile(const QString& path) {
  QFile config_file(path);
  if (config_file.open(QIODevice::ReadOnly)) {
    QByteArray json_data = config_file.readAll();
    config_file.close();
    QJsonDocument config_json = QJsonDocument::fromJson(json_data);
    QJsonObject config_top_object = config_json.object();
    QJsonValue tube_directory_value = config_top_object.value("tube_directory");
    if (tube_directory_value.isString()) {
      m_parameters.tube_directory = tube_directory_value.toString();
    }
    #ifdef DEBUG_BUILD
    qDebug() << "Read config file: " << path;
    #endif // DEBUG_BUILD
    emit configMessage(QString("Loaded configuration file: ") + path);
  }
}
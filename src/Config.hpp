// LoadLineCalculator -- Config.hpp
// Author: Will Marshall
// Created: 7/19/2026, 3:36:59 PM

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QStringList>

class Config : public QObject {
  Q_OBJECT

  QMap<QString, QString> m_tube_map;

  public:
    struct Parameters {
      QString tube_directory;
    };

    Config(QObject* parent = nullptr);

    ~Config() noexcept {}

    const Parameters& currentParameters() const noexcept;
    QString tubePath(const QString& name) const;

    Q_SIGNAL void configMessage(QString msg);
    Q_SIGNAL void tubeDirectoryParsed(QStringList names);

    Q_SLOT void parseTubeDirectory();

  private:
    Parameters m_parameters;

    Parameters createDefaultConfigFile(const QString& path);
    void readConfigFile(const QString& path);
};

#endif // CONFIG_HPP
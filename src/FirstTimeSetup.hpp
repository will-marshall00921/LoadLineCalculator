// LoadLineCalculator -- FirstTimeSetup.hpp
// Author: Will Marshall
// Created: 8/10/2026, 10:12:40 PM

#ifndef FIRSTTIMESETUP_HPP
#define FIRSTTIMESETUP_HPP

#include <QtCore/QString>
#include <QtWidgets/QProgressDialog>
#include <QtCore/QMap>
#include <QtCore/QJsonDocument>

class FirstTimeSetup {
  QString appDataDirPath;
  QString tubeDefaultDirPath;
  QString packedTubesPath;

  public:
    explicit FirstTimeSetup();

    ~FirstTimeSetup();

    bool necessary() const;

    bool perform();

    static bool packDirectory(
      const QString& sourcePath,
      const QString& outputPath
    );

  private:
    static QMap<QString, QJsonDocument> unpackArchive(const QString& archivePath);
    static bool installArchive(
      const QString& archivePath,
      const QString& targetDirPath
    );
    static bool packJsonDocuments(
      const QMap<QString, QJsonDocument>& jsonMap,
      const QString& outputPath
    );
};

#endif // FIRSTTIMESETUP_HPP
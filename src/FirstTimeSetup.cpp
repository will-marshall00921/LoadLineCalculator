// LoadLineCalculator -- FirstTimeSetup.cpp
// Author: Will Marshall
// Created: 8/10/2026, 10:16:59 PM

#include "FirstTimeSetup.hpp"
#include <QtCore/QStandardPaths>
#include <QtWidgets/QApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDataStream>
#include <QtCore/QCborValue>
#ifdef DEBUG_BUILD
#include <QtCore/QDebug>
#endif // DEBUG_BUILD
#include <cstdint>
#include <QtCore/QThread>
#include <QtCore/QStringList>
#include <QtCore/QJsonParseError>
#include <QtCore/QByteArray>
#include <QtCore/QCborParserError>
#include <QtCore/QCoreApplication>

FirstTimeSetup::FirstTimeSetup() {
  appDataDirPath = QStandardPaths::writableLocation(
    QStandardPaths::StandardLocation::AppDataLocation
  );
  tubeDefaultDirPath = (appDataDirPath + "/tubes");
  packedTubesPath = (
    QApplication::applicationDirPath() + "/tubes.bin"
  );
}

FirstTimeSetup::~FirstTimeSetup() {}

bool FirstTimeSetup::necessary() const {
  return (!QDir(tubeDefaultDirPath).exists());
}

bool FirstTimeSetup::perform() {
  if (!QFileInfo::exists(packedTubesPath)) { return false; }
  return installArchive(
    packedTubesPath,
    tubeDefaultDirPath
  );
}

bool FirstTimeSetup::packDirectory(
  const QString& sourcePath,
  const QString& outputPath
) {
  QDir sourceDir(sourcePath);
  if (!sourceDir.exists()) {
    #ifdef DEBUG_BUILD
    qWarning() << "Source directory '" << sourcePath << "' does not exist";
    #endif // DEBUG_BUILD
    return false;
  }
  QStringList jsonFiles = sourceDir.entryList(
    QStringList() << "*.json",
    QDir::Filter::Files
  );
  #ifdef DEBUG_BUILD
  qDebug() << "Packing " << jsonFiles.size() << " JSON files...";
  #endif // DEBUG_BUILD
  QMap<QString, QJsonDocument> jsonMap;
  for (const QString& fileName : jsonFiles) {
    QFile file(sourceDir.filePath(fileName));
    if (!file.open(QIODevice::OpenModeFlag::ReadOnly)) {
      #ifdef DEBUG_BUILD
      qWarning() << "Failed to read '" << sourceDir.filePath(fileName) 
                 << "' - Skipped";
      #endif // DEBUG_BUILD
      continue;
    }
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(
      file.readAll(),
      &parseError
    );
    if (parseError.error != QJsonParseError::ParseError::NoError) {
      #ifdef DEBUG_BUILD
      qWarning() << "Encountered error in '" << sourceDir.filePath(fileName)
                 << "': " << parseError.errorString() << " - Skipped";
      #endif // DEBUG_BUILD
      continue;
    }
    jsonMap.insert(fileName, jsonDoc);
  }
  return packJsonDocuments(jsonMap, outputPath);
}

QMap<QString, QJsonDocument> FirstTimeSetup::unpackArchive(
  const QString& archivePath
) {
  QMap<QString, QJsonDocument> jsonMap;
  QFile file(archivePath);
  if (!file.open(QIODevice::ReadOnly)) {
    #ifdef DEBUG_BUILD
    qWarning() << "Failed to open archived tubes file!";
    #endif // DEBUG_BUILD
    return jsonMap;
  }
  QByteArray compressedData = file.readAll();
  file.close();
  QByteArray decompressedData = qUncompress(compressedData);
  if (decompressedData.isEmpty()) {
    #ifdef DEBUG_BUILD
    qWarning() << "Failed to decompress tubes file (empty file?)";
    #endif // DEBUG_BUILD
    return jsonMap;
  }
  QDataStream inData(&decompressedData, QIODevice::ReadOnly);
  inData.setVersion(QDataStream::Qt_6_7);
  uint32_t magic = 0;
  uint32_t version = 0;
  inData >> magic >> version;
  if (magic != 0x514A534E) {
    #ifdef DEBUG_BUILD
    qWarning() << "Invalid file format or header mismatch!";
    #endif // DEBUG_BUILD
    return jsonMap;
  }
  quint32 count = 0;
  inData >> count;
  for (quint32 i = 0 ; i < count ; i ++) {
    QString keyName;
    QByteArray cborData;
    inData >> keyName >> cborData;
    QCborParserError parserError;
    QCborValue cborValue = QCborValue::fromCbor(cborData, &parserError);
    if (parserError.error != QCborError::Code::NoError) {
      #ifdef DEBUG_BUILD
      qWarning() << "Failed to parse '" << keyName << "': " << parserError.errorString();
      #endif // DEBUG_BUILD
      continue;
    }
    QJsonDocument doc = QJsonDocument::fromVariant(cborValue.toVariant());
    jsonMap.insert(keyName, doc);
  }
  return jsonMap;
}

bool FirstTimeSetup::installArchive(
  const QString& archivePath,
  const QString& targetDirPath
) {
  QProgressDialog progress(
    "Performing first time setup...", 
    "Close", 
    0, 
    TUBE_FILE_COUNT * 2
  );
  progress.setWindowModality(Qt::WindowModality::NonModal);
  progress.setValue(0);
  progress.show();
  QCoreApplication::processEvents();
  QDir targetDir(targetDirPath);
  if (!targetDir.exists()) {
    targetDir.mkpath(".");
  }
  QMap<QString, QJsonDocument> jsonMap = unpackArchive(archivePath);
  if (jsonMap.isEmpty()) {
    #ifdef DEBUG_BUILD
    qDebug() << "No files to install!";
    #endif // DEBUG_BUILD
    return false;
  }
  int progressValue = TUBE_FILE_COUNT;
  progress.setValue(progressValue);
  QCoreApplication::processEvents();
  QThread::currentThread()->msleep(500);
  for (auto it = jsonMap.constBegin() ; it != jsonMap.constEnd() ; it ++) {
    QFile outputFile(targetDir.filePath(it.key()));
    if (outputFile.open(QIODevice::WriteOnly)) {
      outputFile.write(it.value().toJson(QJsonDocument::Indented));
      outputFile.close();
      #ifdef DEBUG_BUILD
      qDebug() << "Installed tube JSON: " << targetDir.filePath(it.key());
      #endif // DEBUG_BUILD
    } else {
      #ifdef DEBUG_BUILD
      qDebug() << "Failed to write file: " << targetDir.filePath(it.key());
      #endif // DEBUG_BUILD
    }
    progressValue ++;
    progress.setValue(progressValue);
    QCoreApplication::processEvents();
    QThread::currentThread()->msleep(250);
  }
  progress.close();
  QCoreApplication::processEvents();
  return true;
}

bool FirstTimeSetup::packJsonDocuments(
  const QMap<QString, QJsonDocument>& jsonMap,
  const QString& outputPath
) {
  QByteArray rawBuffer;
  QDataStream out(&rawBuffer, QIODevice::OpenModeFlag::WriteOnly);
  out.setVersion(QDataStream::Qt_6_7);

  constexpr uint32_t MAGIC = 0x514A534E;
  constexpr uint32_t VERSION = 1;
  out << MAGIC << VERSION;
  out << static_cast<quint32>(jsonMap.size());

  for (auto it = jsonMap.constBegin() ; it != jsonMap.constEnd() ; it ++) {
    const QString& keyName = it.key();
    const QJsonDocument& jsonDoc = it.value();
    QCborValue cborValue = QCborValue::fromJsonValue(jsonDoc.object());
    QByteArray cborData = cborValue.toCbor();
    out << keyName;
    out << cborData;
  }

  QByteArray compressedBuffer = qCompress(
    rawBuffer,
    9
  );
  QFile file(outputPath);
  if (!file.open(QIODeviceBase::OpenModeFlag::WriteOnly)) {
    #ifdef DEBUG_BUILD
    qWarning() << "Failed to write file '" << outputPath << "'";
    #endif // DEBUG_BUILD
    return false;
  }
  file.write(compressedBuffer);
  file.close();
  return true;
}
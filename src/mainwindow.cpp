// LoadLineCalculator -- mainwindow.cpp
// Author: Will Marshall
// Created: 7/18/2026, 5:02:30 PM

#include "mainwindow.hpp"
#include "ui_mainwindow.h"
// #include "ui_mainwindow.temp.h"
#ifdef DEBUG_BUILD
#include <QtCore/QDebug>
#endif // DEBUG_BUILD

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow { parent }
  , ui { new Ui::MainWindow }
  , m_tube_manager { new TubeManager }
  {
  ui->setupUi(this);
  ui->plotWidget->xAxis->setLabel("Plate Voltage [V]");
  ui->plotWidget->yAxis->setLabel("Plate Current [mA]");
  setupTubeManager();
  setupConfig();
  QObject::connect(
    ui->tubeSelectComboBox,
    &QComboBox::currentTextChanged,
    this,
    [&](QString name) {
      emit selectedTube(m_config.tubePath(name));
    }
  );
  m_config.parseTubeDirectory();
}

MainWindow::~MainWindow() noexcept {
  delete ui;
}

void MainWindow::setupTubeManager() {
  QObject::connect(
    m_tube_manager,
    &TubeManager::heaterVoltageChanged,
    this,
    [&](const TubeManager::TubeInfoEntry& entry) {
      updateEntry(ui->heaterVoltageLabel, ui->heaterVoltageValueLabel, entry);
    }
  );
  QObject::connect(
    m_tube_manager,
    &TubeManager::heaterCurrentChanged,
    this,
    [&](const TubeManager::TubeInfoEntry& entry) {
      updateEntry(ui->heaterCurrentLabel, ui->heaterCurrentValueLabel, entry);
    }
  );
  QObject::connect(
    m_tube_manager,
    &TubeManager::gridToPlateCapacitanceChanged,
    this,
    [&](const TubeManager::TubeInfoEntry& entry) {
      updateEntry(ui->cGridToPlateLabel, ui->cGridToPlateValueLabel, entry);
    }
  );
  QObject::connect(
    m_tube_manager,
    &TubeManager::gridToCathodeCapacitanceChanged,
    this,
    [&](const TubeManager::TubeInfoEntry& entry) {
      updateEntry(ui->cGridToCathodeLabel, ui->cGridToCathodeValueLabel, entry);
    }
  );
  QObject::connect(
    m_tube_manager,
    &TubeManager::plateToCathodeCapacitanceChanged,
    this,
    [&](const TubeManager::TubeInfoEntry& entry) {
      updateEntry(ui->cPlateToCathodeLabel, ui->cPlateToCathodeValueLabel, entry);
    }
  );
  QObject::connect(
    m_tube_manager,
    &TubeManager::maxPlateVoltageChanged,
    this,
    [&](const TubeManager::TubeInfoEntry& entry) {
      updateEntry(ui->maxPlateVoltageLabel, ui->maxPlateVoltageValueLabel, entry);
    }
  );
  QObject::connect(
    m_tube_manager,
    &TubeManager::maxScreenVoltageChanged,
    this,
    [&](const TubeManager::TubeInfoEntry& entry) {
      updateEntry(ui->maxScreenVoltageLabel, ui->maxScreenVoltageValueLabel, entry);
    }
  );
  QObject::connect(
    m_tube_manager,
    &TubeManager::maxGridVoltageChanged,
    this,
    [&](const TubeManager::TubeInfoEntry& entry) {
      updateEntry(ui->maxGridVoltageLabel, ui->maxGridVoltageValueLabel, entry);
    }
  );
  QObject::connect(
    m_tube_manager,
    &TubeManager::minGridVoltageChanged,
    this,
    [&](const TubeManager::TubeInfoEntry& entry) {
      updateEntry(ui->minGridVoltageLabel, ui->minGridVoltageValueLabel, entry);
    }
  );
  QObject::connect(
    m_tube_manager,
    &TubeManager::maxPlatePowerDissipationChanged,
    this,
    [&](const TubeManager::TubeInfoEntry& entry) {
      updateEntry(ui->maxPlatePowerLabel, ui->maxPlatePowerValueLabel, entry);
    }
  );
  QObject::connect(
    m_tube_manager,
    &TubeManager::maxScreenPowerDissipationChanged,
    this,
    [&](const TubeManager::TubeInfoEntry& entry) {
      updateEntry(ui->maxScreenPowerLabel, ui->maxScreenPowerValueLabel, entry);
    }
  );
  QObject::connect(
    m_tube_manager,
    &TubeManager::maxHeaterToCathodeVoltageChanged,
    this,
    [&](const TubeManager::TubeInfoEntry& entry) {
      updateEntry(ui->maxHeaterToCathodeVoltageLabel, ui->maxHeaterToCathodeVoltageValueLabel, entry);
    }
  );
  QObject::connect(
    m_tube_manager,
    &TubeManager::minHeaterToCathodeVoltageChanged,
    this,
    [&](const TubeManager::TubeInfoEntry& entry) {
      updateEntry(ui->minHeaterToCathodeVoltageLabel, ui->minHeaterToCathodeVoltageValueLabel, entry);
    }
  );
  QObject::connect(
    m_tube_manager,
    &TubeManager::tubeTypeChanged,
    this,
    [&](const QString& tp) {
      ui->tubeTypeValueLabel->setText(tp);
    }
  );
  QObject::connect(
    this,
    &MainWindow::selectedTube,
    m_tube_manager,
    &TubeManager::loadTubeInfo
  );
  QObject::connect(
    m_tube_manager,
    &TubeManager::tubeManagerStatus,
    this,
    &MainWindow::showStatus
  );
  QObject::connect(
    m_tube_manager,
    &TubeManager::allCurvesLoaded,
    this,
    [&]() {
      const int plottable_count = ui->plotWidget->plottableCount();
      for (int i = 0 ; i < plottable_count ; i ++) {
        ui->plotWidget->plottable(i)->rescaleAxes((i != 0));
      }
      ui->plotWidget->replot();
    }
  );
  QObject::connect(
    m_tube_manager,
    &TubeManager::curveLoaded,
    this,
    [&](const IVCurve& curve) {
      if (!curve.valid) { return; }
      QCPCurve* curveItem = new QCPCurve(ui->plotWidget->xAxis, ui->plotWidget->yAxis);
      curveItem->setPen(QPen(Qt::black));
      curveItem->setData(
        curve.plate_voltages,
        curve.plate_currents
      );
    }
  );
  QObject::connect(
    m_tube_manager,
    &TubeManager::infoCleared,
    this,
    [&]() {
      ui->plotWidget->clearPlottables();
      ui->plotWidget->replot();
    }
  );
}

void MainWindow::setupConfig() {
  QObject::connect(
    &m_config,
    &Config::tubeDirectoryParsed,
    this,
    [&](const QStringList& names) {
      ui->tubeSelectComboBox->clear();
      for (const QString& name : names) {
        ui->tubeSelectComboBox->addItem(name);
      }
      QSignalBlocker tubeSelectBlocker(ui->tubeSelectComboBox);
      ui->tubeSelectComboBox->setCurrentIndex(-1);
    }
  );
  QObject::connect(
    &m_config,
    &Config::configMessage,
    this,
    &MainWindow::showStatus
  );
}

void MainWindow::showStatus(const QString& msg) {
  ui->statusBar->showMessage(msg, 30000);
}
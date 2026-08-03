// LoadLineCalculator -- mainwindow.cpp
// Author: Will Marshall
// Created: 7/18/2026, 5:02:30 PM

#include "mainwindow.hpp"
#include "ui_mainwindow.h"
// #include "ui_mainwindow.temp.h"
#ifdef DEBUG_BUILD
#include <QtCore/QDebug>
#endif // DEBUG_BUILD
#include <QtWidgets/QFileDialog>
#include "TubeDataDialog.hpp"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow { parent }
  , ui { new Ui::MainWindow }
  , m_tube_manager { new TubeManager }
  , m_calculator { new Calculator }
  , m_load_line_curve { nullptr }
  , m_bias_point { nullptr }
  , m_io_range { nullptr }
  {
  ui->setupUi(this);
  ui->plotWidget->xAxis->setLabel("Plate Voltage [V]");
  ui->plotWidget->yAxis->setLabel("Plate Current [mA]");
  ui->loadResistanceScalableSpinBox->setUnitsText(QString::fromStdWString(L"Ω"));
  ui->loadResistanceScalableSpinBox->enablePlaceholder();
  setupTubeManager();
  setupConfig();
  setupCalculator();
  QObject::connect(
    ui->tubeSelectComboBox,
    &QComboBox::currentTextChanged,
    this,
    [&](QString name) {
      emit selectedTube(m_config.tubePath(name));
    }
  );
  QObject::connect(
    ui->actionSavePlot,
    &QAction::triggered,
    this,
    &MainWindow::savePlot
  );
  QObject::connect(
    ui->actionReloadDirectory,
    &QAction::triggered,
    &m_config,
    Config::parseTubeDirectory
  );
  QObject::connect(
    ui->actionMaximize,
    &QAction::triggered,
    this,
    &MainWindow::showMaximized
  );
  QObject::connect(
    ui->actionMinimize,
    &QAction::triggered,
    this,
    &MainWindow::showMinimized
  );
  QObject::connect(
    ui->actionQuit,
    &QAction::triggered,
    this,
    &MainWindow::close
  );
  QObject::connect(
    ui->actionCreateTube,
    &QAction::triggered,
    this,
    &MainWindow::createTube
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
      ui->loadLinePlateSupplyVoltageSpinBox->setValue(
        static_cast<int>(m_tube_manager->currentTubeRatings().max_plate_voltage.value)
      );
      ui->loadLinePlateSupplyVoltageSpinBox->setSuffix(" V");
      setLoadLineLabelEnabled(true);
      setPlateSupplyVoltageEnabled(true);
      ui->loadResistanceLabel->setEnabled(true);
      ui->loadResistanceScalableSpinBox->setEnabled(true);
      QSignalBlocker scalableSpinBoxBlocker(ui->loadResistanceScalableSpinBox);
      ui->loadResistanceScalableSpinBox->setUnitsText(QString::fromStdWString(L"Ω"));
      ui->loadResistanceScalableSpinBox->setScaleIndex(1);
      ui->loadLineLabel->setEnabled(true);
      ui->calculatedValuesLabel->setEnabled(true);
      ui->loadLineModeLabel->setEnabled(true);
      ui->loadLineModeComboBox->setEnabled(true);
      if (m_calculator->currentMode() == Calculator::Mode::Reactive) {
        ui->gridBiasVoltageLabel->setEnabled(true);
        ui->gridBiasVoltageSpinBox->setEnabled(true);
      }
      ui->actionSavePlot->setEnabled(true);
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
      m_load_line_curve = nullptr;
      m_bias_point = nullptr;
      m_io_range = nullptr;
      ui->plotWidget->replot();
      setLoadLineLabelEnabled(false);
      setStageInputLabelEnabled(false);
      setStageOutputLabelEnabled(false);
      setCalculatedValuesLabelEnabled(false);
      setLoadResistanceEnabled(false);
      setGridBiasVoltageEnabled(false);
      setBiasPlateCurrentDisabled();
      setBiasPlateVoltageDisabled();
      setPeakToPeakOutputDisabled();
      setPeakToPeakInputEnabled(false);
      setLoadLineModeEnabled(false);
      ui->actionSavePlot->setEnabled(false);
      setSelfBiasRawResistanceDisabled();
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

void MainWindow::setupCalculator() {
  QObject::connect(
    m_tube_manager,
    &TubeManager::curveLoaded,
    m_calculator,
    &Calculator::loadRawCurve
  );
  QObject::connect(
    m_tube_manager,
    &TubeManager::infoCleared,
    m_calculator,
    &Calculator::clearCurves
  );
  QObject::connect(
    ui->loadResistanceScalableSpinBox,
    &ScalableSpinBoxValue::scaledValueChanged,
    m_calculator,
    &Calculator::setLoadValue
  );
  QObject::connect(
    ui->loadLinePlateSupplyVoltageSpinBox,
    &QSpinBox::valueChanged,
    m_calculator,
    &Calculator::setPlateSupplyVoltage
  );
  QObject::connect(
    ui->gridBiasVoltageSpinBox,
    &QDoubleSpinBox::valueChanged,
    m_calculator,
    &Calculator::setBiasVoltage
  );
  QObject::connect(
    ui->stageInputPeakToPeakSpinBox,
    &QDoubleSpinBox::valueChanged,
    m_calculator,
    &Calculator::setInputVpp
  );
  QObject::connect(
    ui->loadLineModeComboBox,
    &QComboBox::currentIndexChanged,
    m_calculator,
    &Calculator::setMode
  );
  QObject::connect(
    ui->loadLineModeComboBox,
    &QComboBox::currentIndexChanged,
    this,
    [this](int x) {
      if (x == 0) {
        if (m_load_line_curve == nullptr) {
          ui->gridBiasVoltageLabel->setEnabled(false);
          ui->gridBiasVoltageSpinBox->setEnabled(false);
          QSignalBlocker spinBoxBlocker(ui->gridBiasVoltageSpinBox);
          ui->gridBiasVoltageSpinBox->setValue(-100.0);
        }
      } else {
        ui->gridBiasVoltageLabel->setEnabled(true);
        ui->gridBiasVoltageSpinBox->setEnabled(true);
      }
    }
  );
  QObject::connect(
    m_calculator,
    &Calculator::plotLoadLine,
    this,
    [this](QVector<double> x, QVector<double> y) {
      if (m_load_line_curve == nullptr) {
        m_load_line_curve = reinterpret_cast<void*>(
          new QCPCurve(ui->plotWidget->xAxis, ui->plotWidget->yAxis)
        );
      } 
      reinterpret_cast<QCPCurve*>(m_load_line_curve)->setData(x, y);
      reinterpret_cast<QCPCurve*>(m_load_line_curve)->setPen(QPen(Qt::red));
      ui->plotWidget->replot();
      QSignalBlocker biasVoltageSpinBoxBlocker(ui->gridBiasVoltageSpinBox);
      if (m_calculator->currentMode() != Calculator::Mode::Reactive) {
        ui->gridBiasVoltageSpinBox->setValue(-100.0);
        ui->gridBiasVoltageSpinBox->setSuffix(" V");
        ui->gridBiasVoltageSpinBox->setSpecialValueText("--");
        ui->gridBiasVoltageLabel->setEnabled(true);
        ui->gridBiasVoltageSpinBox->setEnabled(true);
      }
    }
  );
  QObject::connect(
    m_calculator,
    &Calculator::plotBiasPoint,
    this,
    [this](double x, double y) {
      if (m_bias_point == nullptr) {
        m_bias_point = new QCPCurve(ui->plotWidget->xAxis, ui->plotWidget->yAxis);
        QCPCurve* bias_point_graph = reinterpret_cast<QCPCurve*>(m_bias_point);
        bias_point_graph->setLineStyle(QCPCurve::LineStyle::lsNone);
        bias_point_graph->setScatterStyle(QCPScatterStyle::ssCircle);
      }
      reinterpret_cast<QCPCurve*>(m_bias_point)->setData({x}, {y});
      ui->plotWidget->replot();
      ui->biasPlateCurrentLabel->setEnabled(true);
      ui->biasPlateCurrentValueLabel->setValueAndUnits(
        y,
        "mA"
      );
      ui->biasPlateCurrentValueLabel->setEnabled(true);
      ui->biasPlateVoltageLabel->setEnabled(true);
      ui->biasPlateVoltageValueLabel->setValueAndUnits(
        static_cast<int>(x),
        "V"
      );
      ui->biasPlateVoltageValueLabel->setEnabled(true);
      ui->stageInputPeakToPeakLabel->setEnabled(true);
      ui->stageInputPeakToPeakSpinBox->setEnabled(true);
    }
  );
  QObject::connect(
    m_calculator,
    &Calculator::plotIORange,
    this,
    [this](QVector<double> x,QVector<double> y) {
      if (m_io_range == nullptr) {
        QCPCurve* io_range = new QCPCurve(ui->plotWidget->xAxis, ui->plotWidget->yAxis);
        io_range->setLineStyle(QCPCurve::LineStyle::lsLine);
        io_range->setScatterStyle(QCPScatterStyle::ssDiamond);
        io_range->setPen(QPen(Qt::blue));
        m_io_range = reinterpret_cast<void*>(io_range);
      }
      QCPCurve* io_range = reinterpret_cast<QCPCurve*>(m_io_range);
      io_range->setData(x, y);
      ui->plotWidget->replot();
      ui->stageOutputPeakToPeakLabel->setEnabled(true);
      try {
        const double io_domain = (x.value(1) - x.value(0));
        ui->stageOutputPeakToPeakValueLabel->setValueAndUnits(
          io_domain,
          "V"
        );
        ui->stageOutputPeakToPeakValueLabel->setEnabled(true);
      } catch (std::exception& e) {
        showStatus(
          QString("Encountered an error plotting the input range: ")
            + e.what()
        );
        return;
      }
    }
  );
  QObject::connect(
    m_calculator,
    &Calculator::selfBiasRawResistance,
    this,
    [this](double r, const QString& units) {
      setSelfBiasRawResistanceDisabled(true, r, units); 
    }
  );
  QObject::connect(
    m_calculator,
    &Calculator::calculatorMessage,
    this,
    &MainWindow::showStatus
  );
}

void MainWindow::showStatus(const QString& msg) {
  ui->statusBar->showMessage(msg, 30000);
}

void MainWindow::setLoadLineLabelEnabled(bool on) {
  ui->loadLineLabel->setEnabled(on);
}

void MainWindow::setStageInputLabelEnabled(bool on) {
  ui->stageInputLabel->setEnabled(on);
}

void MainWindow::setStageOutputLabelEnabled(bool on) {
  ui->stageOutputLabel->setEnabled(on);
}

void MainWindow::setCalculatedValuesLabelEnabled(bool on) {
  ui->calculatedValuesLabel->setEnabled(on);
}

void MainWindow::setLoadLineModeEnabled(bool on) {
  ui->loadLineModeLabel->setEnabled(on);
  ui->loadLineModeLabel->setEnabled(on);
}

void MainWindow::setLoadResistanceEnabled(bool on) {
  ui->loadResistanceLabel->setEnabled(on);
  ui->loadResistanceScalableSpinBox->setEnabled(on);
  if (!on) { ui->loadResistanceScalableSpinBox->enablePlaceholder(); }
}

void MainWindow::setPlateSupplyVoltageEnabled(bool on) {
  ui->loadLinePlateSupplyVoltageLabel->setEnabled(on);
  ui->loadLinePlateSupplyVoltageSpinBox->setEnabled(on);
  if (!on) {
    ui->loadLinePlateSupplyVoltageSpinBox->setSpecialValueText("--");
  }
}

void MainWindow::setGridBiasVoltageEnabled(bool on) {
  ui->gridBiasVoltageLabel->setEnabled(on);
  ui->gridBiasVoltageSpinBox->setEnabled(on);
  if (!on) {
    QSignalBlocker spinBoxBlocker(ui->gridBiasVoltageSpinBox);
    ui->gridBiasVoltageSpinBox->setValue(ui->gridBiasVoltageSpinBox->minimum());
  }
}

void MainWindow::setPeakToPeakInputEnabled(bool on) {
  ui->stageInputPeakToPeakLabel->setEnabled(on);
  ui->stageInputPeakToPeakSpinBox->setEnabled(on);
  if (!on) {
    QSignalBlocker spinBoxBlocker(ui->stageInputPeakToPeakSpinBox);
    ui->stageInputPeakToPeakSpinBox->setValue(ui->stageInputPeakToPeakSpinBox->minimum());
  }
}

void MainWindow::setPeakToPeakOutputDisabled(
  bool on,
  double v
) {
  ui->stageOutputPeakToPeakLabel->setEnabled(on);
  ui->stageOutputPeakToPeakValueLabel->setEnabled(on);
  if (on) {
    ui->stageOutputPeakToPeakValueLabel->setValueAndUnits(v, "V");
  } else {
    ui->stageOutputPeakToPeakValueLabel->setPlaceholderEnabled(!on);
  }
}

void MainWindow::setBiasPlateCurrentDisabled(
  bool on,
  double i
) {
  ui->biasPlateCurrentLabel->setEnabled(on);
  ui->biasPlateCurrentValueLabel->setEnabled(on);
  if (on) {
    ui->biasPlateCurrentValueLabel->setValueAndUnits(i, "mA");
  } else {
    ui->biasPlateCurrentValueLabel->setPlaceholderEnabled(!on);
  }
}

void MainWindow::setBiasPlateVoltageDisabled(
  bool on,
  double v
) {
  ui->biasPlateVoltageLabel->setEnabled(on);
  ui->biasPlateVoltageValueLabel->setEnabled(on);
  if (on) {
    ui->biasPlateVoltageValueLabel->setValueAndUnits(v, "V");
  } else {
    ui->biasPlateVoltageValueLabel->setPlaceholderEnabled(!on);
  }
}

void MainWindow::setApproxOutputPowerDisabled(
  bool on,
  double p
) {
  ui->approxOutputPowerLabel->setEnabled(on);
  ui->approxOutputPowerValueLabel->setEnabled(on);
  if (on) {
    ui->approxOutputPowerValueLabel->setValueAndUnits(p, "W");
  } else {
    ui->approxOutputPowerValueLabel->setPlaceholderEnabled(!on);
  }
}

void MainWindow::setMaxPlateDissipationDisabled(
  bool on,
  double p
) {
  ui->calculatedMaxPlateDissipationLabel->setEnabled(on);
  ui->calculatedMaxPlateDissipationValueLabel->setEnabled(on);
  if (on) {
    ui->calculatedMaxPlateDissipationValueLabel->setValueAndUnits(p, "W");
  } else {
    ui->calculatedMaxPlateDissipationValueLabel->setPlaceholderEnabled(!on);
  }
}

void MainWindow::setMaxScreenDissipationDisabled(
  bool on,
  double p
) {
  ui->calculatedMaxScreenDissipationValueLabel->setEnabled(on);
  ui->calculatedMaxScreenDissipationValueLabel->setEnabled(on);
  if (on) {
    ui->calculatedMaxScreenDissipationValueLabel->setValueAndUnits(p, "W");
  } else {
    ui->calculatedMaxScreenDissipationValueLabel->setPlaceholderEnabled(!on);
  }
}

void MainWindow::setSelfBiasRawResistanceDisabled(
  bool on,
  double r,
  QString units
) {
  ui->selfBiasResistanceLabel->setEnabled(on);
  ui->selfBiasResistanceValueLabel->setEnabled(on);
  if (on) {
    ui->selfBiasResistanceValueLabel->setValueAndUnits(r, units);
  } else {
    ui->selfBiasResistanceValueLabel->setPlaceholderEnabled(!on);
  }
}

void MainWindow::savePlot() {
  QString save_path = QFileDialog::getSaveFileName(
    this,
    "Save As",
    m_config.currentParameters().last_plot_save_dir + "/plot.png",
    "PNG (*.png)"
  );
  const bool path_given = (!save_path.isEmpty());
  if ((path_given) && (!ui->plotWidget->savePng(save_path))) {
    showStatus(QString("Failed to save PNG image: '") + save_path + "'!");
  } else if (path_given){
    showStatus(QString("Saved plot PNG: '") + save_path + "'!");
    QString save_dir = QFileInfo(save_path).absolutePath();
    m_config.updateLastPlotSaveDir(save_dir);
  }
}

void MainWindow::createTube() {
  TubeDataDialog* tubeDialog = new TubeDataDialog(this);
  tubeDialog->setModal(true);
  tubeDialog->setTubeDirectory(
    m_config.currentParameters().tube_directory
  );
  QObject::connect(
    tubeDialog,
    &TubeDataDialog::statusMessage,
    this,
    &MainWindow::showStatus
  );
  tubeDialog->exec();
}
// LoadLineCalculator -- mainwindow.hpp
// Author: Will Marshall
// Created: 7/18/2026, 5:00:04 PM

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QLabel>
#include "BaseValueLabel.hpp"
#include "TubeManager.hpp"
#include "Config.hpp"
#include "Calculator.hpp"
#include <QtCore/QStringLiteral>

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

  Ui::MainWindow* ui;
  TubeManager* m_tube_manager;
  Config m_config;
  Calculator* m_calculator;
  void* m_load_line_curve;
  void* m_bias_point;
  void* m_io_range;

  public:
    MainWindow(QWidget* parent = nullptr);

    ~MainWindow() noexcept;

    Q_SIGNAL void selectedTube(QString path);

  private:
    template <typename _Tp>
    void updateEntry(
      QLabel* label,
      BaseValueLabel<_Tp>* valueLabel,
      const TubeManager::TubeInfoEntry& entry
    ) {
      label->setEnabled(entry.valid);
      valueLabel->setPlaceholderEnabled(!entry.valid);
      valueLabel->setEnabled(entry.valid);
      if (entry.valid) {
        valueLabel->setValueAndUnits(
          static_cast<_Tp>(entry.value),
          entry.units
        );
      }
    }
    void setupTubeManager();
    void setupConfig();
    void setupCalculator();

    Q_SLOT void showStatus(const QString& msg);

    void setLoadLineLabelEnabled(bool on);
    void setStageInputLabelEnabled(bool on);
    void setStageOutputLabelEnabled(bool on);
    void setCalculatedValuesLabelEnabled(bool on);
    void setLoadLineModeEnabled(bool on);
    void setLoadResistanceEnabled(bool on);
    void setPlateSupplyVoltageEnabled(bool on);
    void setGridBiasVoltageEnabled(bool on);
    void setPeakToPeakInputEnabled(bool on);
    void setPeakToPeakOutputDisabled(
      bool on = false,
      double v = 0.0
    );
    void setBiasPlateCurrentDisabled(
      bool on = false,
      double i = 0.0
    );
    void setBiasPlateVoltageDisabled(
      bool on = false,
      double v = 0.0
    );
    void setApproxOutputPowerDisabled(
      bool on = false,
      double p = 0.0
    );
    void setMaxPlateDissipationDisabled(
      bool on = false,
      double p = 0.0
    );
    void setMaxScreenDissipationDisabled(
      bool on = false,
      double p = 0.0
    );
    void setSelfBiasRawResistanceDisabled(
      bool on = false,
      double r = 0.0,
      QString units = "\u03A9"
    );

    void savePlot();

    Q_SLOT void createTube();

    Q_SLOT void openTubeDirectory();
};

#endif // MAINWINDOW_HPP
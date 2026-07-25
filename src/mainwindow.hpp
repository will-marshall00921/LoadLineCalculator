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
};

#endif // MAINWINDOW_HPP
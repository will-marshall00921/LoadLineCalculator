// LoadLineCalculator -- TubeDataDialog.hpp
// Author: Will Marshall
// Created: 8/2/2026, 5:33:35 PM

#ifndef TUBEDATADIALOG_HPP
#define TUBEDATADIALOG_HPP

#include <QtWidgets/QDialog>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QVector>
#include <QtCore/QPair>

namespace Ui {
  class TubeDataDialog;
}

class TubeDataDialog : public QDialog {
  Q_OBJECT

  Ui::TubeDataDialog* ui;

  QString m_tube_dir;

  QString m_tube_name;
  int m_tube_type;
  double m_heater_voltage_v;
  double m_heater_current;
  QString m_heater_current_units;
  double m_c_g_a_pf;
  bool m_c_g_a_enabled;
  double m_c_g_k_pf;
  bool m_c_g_k_enabled;
  double m_c_a_k_pf;
  bool m_c_a_k_enabled;
  int m_max_plate_voltage_v;
  int m_max_screen_voltage_v;
  bool m_max_screen_voltage_enabled;
  double m_max_grid_voltage_v;
  bool m_max_grid_voltage_enabled;
  double m_min_grid_voltage_v;
  bool m_min_grid_voltage_enabled;
  double m_max_plate_power_w;
  bool m_max_plate_power_enabled;
  double m_max_screen_power_w;
  bool m_max_screen_power_enabled;
  double m_max_heater_cathode_voltage_v;
  bool m_max_heater_cathode_voltage_enabled;
  double m_min_heater_cathode_voltage_v;
  bool m_min_heater_cathode_voltage_enabled;
  QString m_csv_path;
  bool m_csv_path_enabled;
  QMap<
    double,
    QVector<
      QPair<
        double,
        double
      >
    >
  > m_transfer_curve_map;
  int m_csv_current_units;

  public:
    TubeDataDialog(QWidget* parent = nullptr);

    ~TubeDataDialog() noexcept;

    Q_SIGNAL void statusMessage(QString msg);

    Q_SLOT void setTubeDirectory(const QString& dir);
    
    Q_SLOT virtual void accept() override;

  private:  
    Q_SLOT QString setTubeName(const QString& name);
    Q_SLOT int setTubeType(int type);

    Q_SLOT double setHeaterVoltage(double v);
    Q_SLOT double setHeaterCurrent(double i);
    Q_SLOT void setHeaterCurrentUnits(double scale);

    Q_SLOT double setGridToPlateCapacitance(double c);
    Q_SLOT void setGridToPlateCapacitanceEnabled(bool enabled);
    Q_SLOT double setGridToCathodeCapacitance(double c);
    Q_SLOT void setGridToCathodeCapacitanceEnabled(bool enabled);
    Q_SLOT double setPlateToCathodeCapacitance(double c);
    Q_SLOT void setPlateToCathodeCapacitanceEnabled(bool enabled);

    Q_SLOT int setMaxPlateVoltage(int v);
    Q_SLOT int setMaxScreenVoltage(int v);
    Q_SLOT void setMaxScreenVoltageEnabled(bool enabled);
    Q_SLOT double setMaxGridVoltage(double v);
    Q_SLOT void setMaxGridVoltageEnabled(bool enabled);
    Q_SLOT double setMinGridVoltage(double v);
    Q_SLOT void setMinGridVoltageEnabled(bool enabled);
    Q_SLOT double setMaxPlatePowerDissipation(double p);
    Q_SLOT void setMaxPlatePowerDissipationEnabled(bool enabled);
    Q_SLOT double setMaxScreenPowerDissipation(double p);
    Q_SLOT void setMaxScreenPowerDissipationEnabled(bool enabled);
    Q_SLOT int setMaxHeaterToCathodeVoltage(int v);
    Q_SLOT void setMaxHeaterToCathodeVoltageEnabled(bool enabled);
    Q_SLOT int setMinHeaterToCathodeVoltage(int v);
    Q_SLOT void setMinHeaterToCathodeVoltageEnabled(bool enabled);

    Q_SLOT QString setTransferCurveCSVPath(const QString& path);
    Q_SLOT void browseTransferCurveCSVPath();
    Q_SLOT void setTransferCurveCSVPathEnabled(bool enabled);
    Q_SLOT void setTransferCurveCSVCurrentUnits(int unitsIndex);

    Q_SLOT void saveTubeFile();

    void loadTransferCurveCSV(const QString& path);

    Q_SLOT void showCSVHelp();
};

#endif // TUBEDATADIALOG_HPP
// LoadLineCalculator -- Calculator.hpp
// Author: Will Marshall
// Created: 7/22/2026, 10:11:06 PM

#ifndef CALCULATOR_HPP
#define CALCULATOR_HPP

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QVector>
#include "IVCurve.hpp"
#include <QtCore/QPair>
#include <QtCore/QString>
#include "InterpolatedCurve.hpp"

class Calculator : public QObject {
  Q_OBJECT

  QThread* m_thread;
  
  QVector<IVCurve> m_raw_curves;
  QVector<InterpolatedCurve> m_interp_curves;
  bool m_load_ohms_valid;
  double m_load_ohms;
  bool m_plate_supply_valid;
  double m_plate_supply;
  bool m_bias_valid;
  double m_bias;
  bool m_input_vpp_valid;
  double m_input_vpp;
  double m_calculated_ll_slope;
  double m_calculated_ll_intercept;
  double m_calculated_bias_va;
  double m_calculated_bias_ia;
  double m_calculated_output_max_ia;
  double m_calculated_output_min_ia;
  double m_calculated_output_max_va;
  double m_calculated_output_min_va;

  public:
    enum class Mode {
      Resistive = 0,
      Reactive = 1,
    };

    Calculator();

    ~Calculator() noexcept;

    Mode currentMode() const noexcept;

    Q_SIGNAL void plotLoadLine(QVector<double> voltages_v, QVector<double> currents_ma);
    Q_SIGNAL void plotBiasPoint(double v, double i);
    Q_SIGNAL void plotIORange(
      QVector<double> voltages_v,
      QVector<double> currents_ma
    );

    Q_SIGNAL void approximatedOutputPower(double p);
    Q_SIGNAL void selfBiasRawResistance(double r, QString units);
    Q_SIGNAL void selfBiasStandardResistance(double r, QString units);
    
    Q_SIGNAL void calculatorMessage(QString msg);

    Q_SLOT void loadRawCurve(const IVCurve& rawCurve);
    Q_SLOT void clearCurves();

    Q_SLOT double setLoadValue(double r);
    
    Q_SLOT double setPlateSupplyVoltage(int v);

    Q_SLOT double setBiasVoltage(double v);

    Q_SLOT double setInputVpp(double vpp);
    
    Q_SLOT Mode setMode(int mode);

  private:
    Mode m_mode;

    static bool is_linear_intersect(
      double x0, double x1,
      double m0, double b0,
      double m1, double b1
    );

    static QPair<double, double> linear_intersect(
      double m0, double b0,
      double m1, double b1
    );

    QPair<int, int> find_bounding_grid_curves(double tgt_vg) const;

    void calculateLoadLine();

    void calculateBiasIntersection();

    void calculateIORange();

    bool canCalculateLoadLine() const noexcept;
    bool canCalculateBiasIntersection() const noexcept;
    bool canCalculateIORange() const noexcept;

    QPair<double, double> loadLineIntersect(
      double vg, 
      bool* intersect_found = nullptr
    ) const;

    double verticalIntersect(
      double vg,
      bool* intersect_found = nullptr
    ) const;
};

#endif // CALCULATOR_HPP
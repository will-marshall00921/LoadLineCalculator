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
  double m_load_ohms;
  double m_plate_supply;

  public:
    enum class Mode {
      Resistive = 0,
      Reactive = 1,
    };

    Calculator();

    ~Calculator() noexcept;

    Q_SIGNAL void plotLoadLine(QVector<double> voltages_v, QVector<double> currents_ma);
    
    Q_SIGNAL void calculatorMessage(QString msg);

    Q_SLOT void loadRawCurve(const IVCurve& rawCurve);
    Q_SLOT void clearCurves();

    Q_SLOT double setLoadValue(double r);
    
    Q_SLOT double setPlateSupplyVoltage(int v);
    
    Q_SLOT Mode setMode(Mode mode);

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

    QPair<int, int> find_bounding_grid_curves(double tgt_vg);

    void calculateLoadLine();
};

#endif // CALCULATOR_HPP
// LoadLineCalculator -- IVCurve.hpp
// Author: Will Marshall
// Created: 7/19/2026, 6:19:04 PM

#ifndef IVCURVE_HPP
#define IVCURVE_HPP

#include <QtCore/QVector>
#include <QtCore/QJsonObject>

struct IVCurve {
  bool valid;
  double grid_voltage;
  int point_count;
  QVector<double> plate_voltages;
  QVector<double> plate_currents;

  IVCurve() noexcept;
  IVCurve(const IVCurve& other);
  IVCurve(IVCurve&& other) noexcept;

  ~IVCurve() noexcept;

  IVCurve& operator = (const IVCurve& other);
  IVCurve& operator = (IVCurve&& other) noexcept;

  static IVCurve fromJson(const QJsonObject& json);
};

#endif // IVCURVE_HPP
// LoadLineCalculator -- InterpolatedCurve.hpp
// Author: Will Marshall
// Created: 7/23/2026, 6:17:19 PM

#ifndef INTERPOLATEDCURVE_HPP
#define INTERPOLATEDCURVE_HPP

#include <QtCore/QVector>
#include "IVCurve.hpp"

struct InterpolatedCurve {
  struct Section {
    double slope;
    double intercept;
    double lower_bound;
    double upper_bound;
  };
  
  double vg;
  QVector<Section> sections;
  double curve_lower_bound;
  double curve_upper_bound;

  double f(double x) const;

  static InterpolatedCurve fromRawCurve(const IVCurve& curve);
};

#endif // INTERPOLATEDCURVE_HPP
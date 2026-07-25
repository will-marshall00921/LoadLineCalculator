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

  double slopeAt(double x) const;
  double interceptAt(double x) const;

  static InterpolatedCurve fromRawCurve(const IVCurve& curve);
  
  static InterpolatedCurve interpolate(
    const InterpolatedCurve& curve1,
    const InterpolatedCurve& curve2,
    double tgt_vg,
    double resolution = 0.0
  );
  inline InterpolatedCurve interpolateWith(
    const InterpolatedCurve& other,
    double tgt_vg,
    double resolution = 0.0
  ) const {
    return InterpolatedCurve::interpolate(*this, other, tgt_vg, resolution);
  }

  private:
    static bool is_within_section(const Section& s, double x);
};

#endif // INTERPOLATEDCURVE_HPP
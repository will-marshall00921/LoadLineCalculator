// LoadLineCalculator -- InterpolatedCurve.cpp
// Author: Will Marshall
// Created: 7/23/2026, 6:21:49 PM

#include "InterpolatedCurve.hpp"
#include <stdexcept>

double InterpolatedCurve::f(double x) const {
  auto linear = [](double _m, double _x, double _b) {
    return ((_m*_x) + _b);
  };
  if (x < curve_lower_bound) {
    // interpolate using the first slope and intercept
    const Section& first_section = sections.front();
    return linear(first_section.slope, x, first_section.intercept);
  } else if (x > curve_upper_bound) {
    // interpolate using the last slope and intercept
    const Section& last_section = sections.back();
    return linear(last_section.slope, x, last_section.intercept);
  } else {
    // search for the section containing the given x value
    int section_index = -1;
    for (int i = 0 ; i < sections.size() ; i ++) {
      if ((x >= sections[i].lower_bound) && (x <= sections[i].upper_bound)) {
        section_index = i;
        break;
      }
    }
    if (section_index == -1) {
      throw std::runtime_error("Failed to find a matching section!");
    }
    const Section& section = sections.at(section_index);
    return linear(section.slope, x, section.intercept);
  }
}

InterpolatedCurve InterpolatedCurve::fromRawCurve(const IVCurve& curve) {
  InterpolatedCurve ret;
  ret.curve_lower_bound = curve.plate_voltages.front();
  ret.curve_upper_bound = curve.plate_voltages.back();
  ret.vg = curve.grid_voltage;
  const int num_interps = (curve.plate_voltages.size() - 1);
  for (int i = 0 ; i < num_interps ; i ++) {
    InterpolatedCurve::Section section;
    const int ip1 = (i+1);
    section.lower_bound = curve.plate_voltages[i];
    section.upper_bound = curve.plate_voltages[ip1];
    section.slope = (
      (curve.plate_currents[ip1] - curve.plate_currents[i])
        / (curve.plate_voltages[ip1] - curve.plate_voltages[i])
    );
    section.intercept = (
      curve.plate_currents[i] 
        - (section.slope * curve.plate_voltages[i])
    );
    ret.sections.append(section);
  }
  return ret;
}
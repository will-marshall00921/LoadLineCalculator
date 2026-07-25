// LoadLineCalculator -- InterpolatedCurve.cpp
// Author: Will Marshall
// Created: 7/23/2026, 6:21:49 PM

#include "InterpolatedCurve.hpp"
#include <stdexcept>
#include <algorithm>
#include <cmath>

constexpr double linear(double m, double x, double b) {
  return ((m*x) + b);
}

double InterpolatedCurve::f(double x) const {
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
      if (
        (x >= sections[i].lower_bound) 
          && (x <= sections[i].upper_bound)
      ) {
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

double InterpolatedCurve::slopeAt(double x) const {
  if (x <= curve_lower_bound) {
    // give the first slope
    return sections.front().slope;
  } else if (x >= curve_upper_bound) {
    // give the last slope
    return sections.back().slope;
  } else {
    // search for the section containing the given x value
    // NOTE: quickly determine if iterating forwards or backwards is faster
    const double norm_x = (
      (x - curve_lower_bound) 
        / (curve_upper_bound - curve_lower_bound)
    );
    if (norm_x < 0.5) {
      // iterate forwards
      for (auto iter = sections.cbegin() ; iter != sections.cend() ; iter++) {
        const Section& section = (*iter);
        if (is_within_section(section, x)) {
          return section.slope;
        }
      }
    } else {
      // iterate backwards
      for (auto riter = sections.crbegin() ; riter != sections.crend() ; riter++) {
        const Section& section = (*riter);
        if (is_within_section(section, x)) {
          return section.slope;
        }
      }
    }
    // failed to find a match, return 0
    return 0.;
  }
}

double InterpolatedCurve::interceptAt(double x) const {
  if (x <= curve_lower_bound) {
    // give the first intercept
    return sections.front().intercept;
  } else if (x >= curve_upper_bound) {
    // give the last intercept
    return sections.back().intercept;
  } else {
    // search for the section containing the given x value
    // NOTE: quickly determine if iterating forwards or backwards is faster
    const double norm_x = (
      (x - curve_lower_bound) 
        / (curve_upper_bound - curve_lower_bound)
    );
    if (norm_x < 0.5) {
      // iterate forwards
      for (auto iter = sections.cbegin() ; iter != sections.cend() ; iter++) {
        const Section& section = (*iter);
        if (is_within_section(section, x)) {
          return section.intercept;
        }
      }
    } else {
      // iterate backwards
      for (auto riter = sections.crbegin() ; riter != sections.crend() ; riter++) {
        const Section& section = (*riter);
        if (is_within_section(section, x)) {
          return section.intercept;
        }
      }
    }
    // failed to find a match, return 0
    return 0.;
  }
}

InterpolatedCurve InterpolatedCurve::fromRawCurve(
  const IVCurve& curve
) {
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

InterpolatedCurve InterpolatedCurve::interpolate(
  const InterpolatedCurve& curve1,
  const InterpolatedCurve& curve2,
  double tgt_vg,
  double resolution
) {
  InterpolatedCurve interpolated;
  interpolated.curve_lower_bound = std::max(
    curve1.curve_lower_bound, 
    curve2.curve_lower_bound
  );
  interpolated.curve_upper_bound = std::min(
    curve1.curve_upper_bound,
    curve2.curve_upper_bound
  );
  const double domain = (
    interpolated.curve_upper_bound 
      - interpolated.curve_lower_bound
  );
  if (
    (resolution <= 0.0)
      || (resolution > domain)
  ) {
    resolution = (domain / 1E3);
  }
  double current_va = interpolated.curve_lower_bound;
  const double end_va = (interpolated.curve_upper_bound - resolution);
  const double weight1 = std::abs(tgt_vg - curve2.vg);
  const double weight2 = std::abs(curve1.vg - tgt_vg);
  const double weight_sum = (weight1+weight2);
  do {
    if (current_va > interpolated.curve_upper_bound) { 
      current_va = interpolated.curve_upper_bound; 
    }
    InterpolatedCurve::Section interpolated_section;
    const double next_va = (current_va + resolution);
    interpolated_section.lower_bound = current_va;
    interpolated_section.upper_bound = next_va;
    auto weighted_avg = [](
      double x1, double x2, 
      double w1, double w2, double w1pw2
    ) {
      return (((x1*w1) + (x2*w2)) / (w1pw2));
    };
    interpolated_section.slope = weighted_avg(
      curve1.slopeAt(current_va), curve2.slopeAt(current_va),
      weight1, weight2, weight_sum
    );
    interpolated_section.intercept = weighted_avg(
      curve1.interceptAt(current_va), curve2.interceptAt(current_va),
      weight1, weight2, weight_sum
    );
    interpolated.sections.append(interpolated_section);
    current_va = next_va;
  } while (current_va < interpolated.curve_upper_bound);
  return interpolated;
}

bool InterpolatedCurve::is_within_section(
  const InterpolatedCurve::Section& s,
  double x
) {
  return ((x >= s.lower_bound) && (x <= s.upper_bound));
}
// LoadLineCalculator -- eeutils.hpp
// Author: Will Marshall
// Created: 5/28/2026, 7:20:21 PM

#ifndef EEUTILS_HPP
#define EEUTILS_HPP

#include <vector>
#include <utility>
#include <string>

enum ESeries : int {
  E6 = 0,
  E12 = 1,
  E24 = 2,
  E48 = 3,
  E96 = 4,
  E192 = 5,
};

enum Units : int {
  Milli = 0,
  DefaultUnits = 1,
  Kilo = 2,
  Mega = 3,
};

std::vector<int> series_base_values(int series);

double units_scale(int units);

std::pair<double, int> to_nearest_series_value(
  double x, int units, int series,
  int* nearest_index = nullptr,
  int* multiplier = nullptr
);

std::string to_units_str(int units);

#endif // EEUTILS_HPP
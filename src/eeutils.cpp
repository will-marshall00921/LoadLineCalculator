// LoadLineCalculator -- eeutils.cpp
// Author: Will Marshall
// Created: 5/28/2026, 7:30:54 PM

#include "eeutils.hpp"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>

std::vector<int> series_base_values(int series) {
  switch (series) {
    case E6:
      return {
        10,   15,   22,   33,   47,   68
      };
    case E12:
      return {
        10,   12,   15,   18,   22,   27,   33,   39,   47,   56,   68,   82
      };
    case E24:
      return {
        10,   11,   12,   13,   15,   16,   18,   20,   22,   24,   27,   30,
        33,   36,   39,   43,   47,   51,   56,   62,   68,   75,   82,   91
      };
    case E48:
      return {
        100,  105,  110,  115,  121,  127,  133,  140,  147,  154,  162,  169,
        178,  187,  196,  205,  215,  226,  237,  249,  261,  274,  287,  301,
        316,  332,  348,  365,  383,  402,  422,  442,  464,  487,  511,  536,
        562,  590,  619,  649,  681,  715,  750,  787,  825,  866,  909,  953 
      };
    case E96:
      return {
        100,  102,	105,	107,	110,	113,  115,	118,	121,	124,	127,	130,
        133,	137,	140,	143,	147,	150,  154,	158,	162,	165,	169,	174,
        178,	182,	187,	191,	196,	200,  205,	210,	215,	221,	226,	232,
        237, 	243,  249,	255,	261,	267,  274,	280,	287,	294,	301,	309,
        316,	324,	332,	340,	348,	357,  365,	374,	383,	392,	402,	412,
        422,	432,	442,	453,	464,	475,  487,	499,	511,	523,	536,	549,
        562,	576,	590,	604,	619,	634,  649,	665,	681,	698,	715,	732,
        750,	768,	787,	806,	825,	845,  866,	887,	909,	931,	953,	976
      };
    case E192:
      return {
        100,	101,	102,	104,	105,	106,	107,	109,	110,	111,	113,	114,
        115,	117,	118,	120,	121,	123,	124,	126,	127,	129,	130,	132,
        133,	135,	137,	138,	140,	142,	143,	145,	147,	149,	150,	152,
        154,	156,	158,	160,	162,	164,	165,	167,	169,	172,	174,	176,
        178,	180,	182,	184,	187,	189,	191,	193,	196,	198,	200,	203,
        205,	208,	210,	213,	215,	218,	221,	223,	226,	229,	232,	234,
        237,	240,	243,	246,	249,	252,	255,	258,	261,	264,	267,	271,
        274,	277,	280,	284,	287,	291,	294,	298,	301,	305,	309,	312,
        316,	320,	324,	328,	332,	336,	340,	344,	348,	352,	357,	361,
        365,	370,	374,	379,	383,	388,	392,	397,	402,	407,	412,	417,
        422,	427,	432,	437,	442,	448,	453,	459,	464,	470,	475,	481,
        487,	493,	499,	505,	511,	517,	523,	530,	536,	542,	549,	556,
        562,	569,	576,	583,	590,	597,	604,	612,	619,	626,	634,	642,
        649,	657,	665,	673,	681,	690,	698,	706,	715,	723,	732,	741,
        750,	759,	768,	777,	787,	796,	806,	816,	825,	835,	845,	856,
        866,	876,	887,	898,	909,	920,	931,	942,	953,	965,	976,	988,
      };
    default:
      return std::vector<int>();
  }
}

double units_scale(int units) {
  return std::pow(10., static_cast<double>(3*(-1 + units)));
}

std::pair<int, int> most_sig_3(double x, int units) {
  if (x == 0.) {
    return { 0, units }; 
  }
  x *= units_scale(units);
  std::stringstream ss;
  ss << std::scientific << std::setprecision(2) << x;
  std::string s = ss.str();
  size_t e_pos = s.find('e');
  const int out_units = (std::stoi(s.substr(e_pos + 1)) - 2);
  std::string ms3_str = s.substr(0, e_pos);
  ms3_str.erase(
    ms3_str.find('.'),
    1
  );
  return {std::stoi(ms3_str), out_units};
}

std::pair<double, int> to_nearest_series_value(
  double x, int units, int series,
  int* nearest_index,
  int* multiplier
) {
  std::pair<int, int> ms3_pair = most_sig_3(x, units);
  if (series < E48) {
    ms3_pair.first /= 10; // reduce to 2 digits
  }
  const std::vector<int> possible_base_values = series_base_values(series);
  // std::vector<int> diffs;
  // diffs.reserve(possible_base_values.size());
  int min_diff = 9999999;
  int diff;
  unsigned int best_index = 0;
  unsigned int current_index = 0;
  for (int possible_val : possible_base_values) {
    diff = std::abs(ms3_pair.first - possible_val);
    if (diff < min_diff) {
      best_index = current_index;
      min_diff = diff;
    }
    current_index ++;
  }
  if (ms3_pair.second > 6) { // value larger than 9.99e8 (999e6)
    if (nearest_index != nullptr) {
      (*nearest_index) = (possible_base_values.size() - 1);
    }
    if (multiplier != nullptr) {
      (*multiplier) = 6;
    }
    return {
      possible_base_values.back(),
      Mega
    };
  } 
  const double best_value = static_cast<double>(
    possible_base_values[best_index] 
      * (1 + (-(series < E48) & 9))
  );
  if (ms3_pair.second > 3) { // value larger than 9.99e5 (999e3)
    const int mul = (6 - ms3_pair.second);
    if (multiplier != nullptr) {
      (*multiplier) = ms3_pair.second;
    }
    const double final_value = (
      best_value
        / std::pow(10., static_cast<double>(mul))
    );
    if (nearest_index != nullptr) {
      (*nearest_index) = best_index;
    }
    return {
      final_value,
      Mega
    };
  } else if (ms3_pair.second > 0) { // value larger than 9.99e2 (999)
    const int mul = (3 - ms3_pair.second);
    if (multiplier != nullptr) {
      (*multiplier) = ms3_pair.second;
    }
    const double final_value = (
      best_value
        / std::pow(10., static_cast<double>(mul))
    );
    if (nearest_index != nullptr) {
      (*nearest_index) = best_index;
    }
    return {
      final_value,
      Kilo
    };
  } else if (ms3_pair.second > -3) { // value larger than 9.99e-1 (999e-3)
    const int mul = (-ms3_pair.second);
    if (multiplier != nullptr) {
      (*multiplier) = ms3_pair.second;
    }
    const double final_value = (
      best_value
        / std::pow(10., static_cast<double>(mul))
    );
    if (nearest_index != nullptr) {
      (*nearest_index) = best_index;
    }
    return {
      final_value,
      DefaultUnits
    };
  } else if (ms3_pair.second > -6) { // value larger than 9.99e-4 (999e-6)
    const int mul = (-3 - ms3_pair.second);
    if (multiplier != nullptr) {
      (*multiplier) = ms3_pair.second;
    }
    const double final_value = (
      best_value
        / std::pow(10., static_cast<double>(mul))
    );
    if (nearest_index != nullptr) {
      (*nearest_index) = best_index;
    }
    return {
      final_value,
      Milli
    };
  } else {
    if (nearest_index != nullptr) {
      (*nearest_index) = 0;
    }
    if (multiplier != nullptr) {
      (*multiplier) = -3;
    }
    return {
      static_cast<double>(possible_base_values.front() 
        / ((series < E48) ? (10) : (100))),
      Milli
    };
  }
}

std::string to_units_str(int units) {
  switch (units) {
    case Milli:
      return "m\316\251";
    case DefaultUnits:
      return "\316\251";
    case Kilo:
      return "k\316\251";
    case Mega:
      return "M\316\251";
    default:
      return "?\316\251";
  }
}
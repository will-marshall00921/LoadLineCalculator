// LoadLineCalculator -- Calculator.cpp
// Author: Will Marshall
// Created: 7/23/2026, 6:12:12 PM

#include "Calculator.hpp"
#include <algorithm>
#ifdef DEBUG_BUILD
#include <QtCore/QDebug>
#endif // DEBUG_BUILD

Calculator::Calculator()
  : QObject { nullptr }
  , m_thread { new QThread }
  , m_load_ohms_valid { false }
  , m_load_ohms { 0.0 }
  , m_plate_supply_valid { false }
  , m_plate_supply { 0.0 }
  , m_bias_valid { false }
  , m_bias { 0.0 }
  , m_input_vpp_valid { false }
  , m_input_vpp { 0.0 }
  , m_calculated_bias_va { 0.0 }
  , m_calculated_bias_ia { 0.0 }
  {
  this->moveToThread(m_thread);
  m_thread->start();
}

Calculator::~Calculator() noexcept {
  if ((m_thread) && (m_thread->isRunning())) {
    m_thread->requestInterruption();
    m_thread->quit();
    if (!m_thread->wait(5000)) {
      m_thread->terminate();
      m_thread->wait();
    }
    delete m_thread;
  }
}

Calculator::Mode Calculator::currentMode() const noexcept {
  return m_mode;
}

void Calculator::loadRawCurve(const IVCurve& rawCurve) {
  m_raw_curves.append(rawCurve);
  m_interp_curves.append(InterpolatedCurve::fromRawCurve(rawCurve));
}

void Calculator::clearCurves() {
  m_raw_curves.clear();
  m_interp_curves.clear();
  m_load_ohms_valid = false;
  m_plate_supply_valid = false;
  m_bias_valid = false;
  m_input_vpp_valid = false;
  m_calculated_bias_va = 0.0;
  m_calculated_bias_ia = 0.0;
  #ifdef DEBUG_BUILD
  qDebug() << "[Calculator]{clearCurves} Loaded and calculated variables cleared!";
  #endif // DEBUG_BUILD
}

double Calculator::setLoadValue(double r) {
  const double retval = m_load_ohms;
  m_load_ohms = r;
  #ifdef DEBUG_BUILD
  qDebug() << "[Calculator]{setLoadValue} New load value (ohms): " << r;
  #endif // DEBUG_BUILD
  m_load_ohms_valid = true;
  if ((retval != r) && (canCalculateLoadLine())) {
    calculateLoadLine();
  }
  if (canCalculateBiasIntersection()) {
    calculateBiasIntersection();
  }
  if (canCalculateIORange()) {
    calculateIORange();
  }
  return retval;
}

double Calculator::setPlateSupplyVoltage(int v) {
  const double retval = m_plate_supply;
  m_plate_supply = static_cast<double>(v);
  #ifdef DEBUG_BUILD
  qDebug() << "[Calculator]{setPlateSupplyVoltage} New plate supply voltage: "
           << v;
  #endif // DEBUG_BUILD
  m_plate_supply_valid = true;
  const bool val_changed = (retval != m_plate_supply);
  if (val_changed && (canCalculateLoadLine())) {
    calculateLoadLine();
  }
  if (val_changed && canCalculateBiasIntersection()) {
    calculateBiasIntersection();
  }
  if (val_changed && canCalculateIORange()) {
    calculateIORange();
  }
  return retval;
}

double Calculator::setBiasVoltage(double v) {
  const double retval = m_bias;
  m_bias = v;
  #ifdef DEBUG_BUILD
  qDebug() << "[Calculator]{setBiasVoltage} New bias voltage: " << v;
  #endif // DEBUG_BUILD
  m_bias_valid = true;
  const bool val_changed = (retval != v);
  if ((m_mode == Mode::Reactive) && val_changed && canCalculateLoadLine()) {
    calculateLoadLine();
  }
  if (val_changed && (canCalculateBiasIntersection())) {
    calculateBiasIntersection();
  }
  if (val_changed && canCalculateIORange()) {
    calculateIORange();
  }
  return retval;
}

double Calculator::setInputVpp(double vpp) {
  const double retval = m_input_vpp;
  m_input_vpp = vpp;
  #ifdef DEBUG_BUILD
  qDebug() << "[Calculator]{setInputVpp} New input Vpp: " << vpp;
  #endif // DEBUG_BUILD
  m_input_vpp_valid = true;
  const bool val_changed = (retval != vpp);
  if (val_changed && (canCalculateIORange())) {
    calculateIORange();
  }
  return retval;
}

Calculator::Mode Calculator::setMode(int mode) {
  const Calculator::Mode retval = m_mode;
  m_mode = static_cast<Calculator::Mode>(mode);
  #ifdef DEBUG_BUILD
  qDebug() << "[Calculator]{setMode} New mode: " << static_cast<int>(mode);
  #endif // DEBUG_BUILD
  const bool val_changed = (retval != m_mode);
  if (val_changed && canCalculateLoadLine()) {
    calculateLoadLine();
  }
  if (val_changed && canCalculateBiasIntersection()) {
    calculateBiasIntersection();
  }
  if (val_changed && canCalculateIORange()) {
    calculateIORange();
  }
  return retval;
}

bool Calculator::is_linear_intersect(
  double x0, double x1,
  double m0, double b0,
  double m1, double b1
) {
  const double diff_l = (((m0 * x0) + b0) - ((m1 * x0) + b1));
  const double diff_r = (((m0 * x1) + b0) - ((m1 * x1) + b1));
  return (
    (diff_l == 0)
      || (diff_r == 0)
      || ((diff_l < 0.) && (diff_r > 0.))
      || ((diff_l > 0.) && (diff_r < 0.))
  );
}

QPair<double, double> Calculator::linear_intersect(
  double m0, double b0,
  double m1, double b1
) {
  const double intersect_x = ((b1 - b0) / (m0 - m1));
  return {intersect_x, (m0 * intersect_x) + b0};
}

QPair<int, int> Calculator::find_bounding_grid_curves(double tgt_vg) const {
  QPair<int, int> retpair(-1, -1);
  for (int i = 0 ; i < (m_interp_curves.size() - 1) ; i ++) {
    const int ip1 = (i+1);
    if ((i == 0) && (tgt_vg == m_interp_curves[i].vg)) {
      retpair.first = i;
      retpair.second = i;
      break;
    } else if (tgt_vg == m_interp_curves[ip1].vg) {
      retpair.first = ip1;
      retpair.second = ip1;
      break;
    } else if (
      (tgt_vg <= m_interp_curves[i].vg)
        && (tgt_vg >= m_interp_curves[ip1].vg)
    ) {
      retpair.first = i;
      retpair.second = ip1;
      break;
    }
  }
  return retpair;
}

void Calculator::calculateLoadLine() {
  #ifdef DEBUG_BUILD
  qDebug() << "[Calculator]{calculateLoadLine} Begin calculation...";
  #endif // DEBUG_BUILD
  QVector<double> voltages;
  QVector<double> currents;
  if (m_mode == Mode::Reactive) {
    bool found_intersect = false;
    const double reactive_offset = verticalIntersect(m_bias, &found_intersect);
    if (!found_intersect) {
      emit calculatorMessage("Failed to find a intersect for the bias voltage!");
      return;
    }
    double max_ia_ma = (1E3 * m_plate_supply / m_load_ohms);
    m_calculated_ll_slope = (-1. * max_ia_ma / m_plate_supply);
    max_ia_ma += reactive_offset;
    m_calculated_ll_intercept = max_ia_ma;
    voltages.append(0.);
    // voltages.append((max_ia_ma - m_calculated_ll_intercept) / m_calculated_ll_slope);
    voltages.append(m_plate_supply + (-1. * reactive_offset / m_calculated_ll_slope));
    currents.append(max_ia_ma);
    currents.append(0.);
  } else {
    voltages.append(0.);
    voltages.append(m_plate_supply);
    const double max_ia_ma = (1E3 * m_plate_supply / m_load_ohms);
    currents.append(1E3 * m_plate_supply / m_load_ohms);
    currents.append(0.);
    m_calculated_ll_slope = (-1. * max_ia_ma / m_plate_supply);
    m_calculated_ll_intercept = max_ia_ma;
  }
  #ifdef DEBUG_BUILD
  qDebug() << "[Calculator]{calculateLoadLine} End calculation: m="
           << m_calculated_ll_slope << ", b=" << m_calculated_ll_intercept;
  #endif // DEBUG_BUILD
  emit plotLoadLine(voltages, currents);
  emit calculatorMessage(
    QString("Plotting load line for Va=")
      + QString::number(m_plate_supply)
      + " V, Rl="
      + QString::number(m_load_ohms)
      + QString::fromStdWString(L" Ω")
  );
}

void Calculator::calculateBiasIntersection() {
  #ifdef DEBUG_BUILD
  qDebug() << "[Calculator]{calculateBiasIntersection} Begin calculation...";
  #endif // DEBUG_BUILD
  
  bool found_intersect = false;
  QPair<double, double> intersect_point = loadLineIntersect(m_bias, &found_intersect);
  if (!found_intersect) {
    #ifdef DEBUG_BUILD
    qDebug() << "[Calculator]{calculateBiasIntersection} No load line intersection found for vg="
             << m_bias << " V";
    #endif // DEBUG_BUILD
    return;
  }
  m_calculated_bias_va = intersect_point.first;
  m_calculated_bias_ia = intersect_point.second;
  emit plotBiasPoint(m_calculated_bias_va, m_calculated_bias_ia);
  #ifdef DEBUG_BUILD
  qDebug() << "[Calculator]{calculateBiasIntersection} End calculation: va="
           << m_calculated_bias_va << " V, ia=" << m_calculated_bias_ia << "mA";
  #endif // DEBUG_BUILD
  emit calculatorMessage(
    QString("Bias point calculated for vg=")
      + QString::number(m_bias)
      + " V: va="
      + QString::number(m_calculated_bias_va)
      + " V, ia="
      + QString::number(m_calculated_bias_ia)
      + " mA"
  );
  // calculating the self-bias resistor
  double raw_self_bias_resistance_kohms = (
    std::abs(m_bias) 
      / m_calculated_bias_ia
  );
  emit selfBiasRawResistance(raw_self_bias_resistance_kohms*1E3, QString::fromStdWString(L"Ω"));
}

void Calculator::calculateIORange() {
  #ifdef DEBUG_BUILD
  qDebug() << "[Calculator]{calculatorIORange} Begin calculation...";
  #endif // DEBUG_BUILD
  const double halfVpp = (m_input_vpp * 0.5);
  
  const double vg_upper_bound (m_interp_curves.front().vg);
  const double vg_lower_bound = (m_interp_curves.back().vg);

  double vg_upper = (m_bias + halfVpp);
  if (vg_upper > vg_upper_bound) { vg_upper = vg_upper_bound; }
  double vg_lower = (m_bias - halfVpp);
  if (vg_lower < vg_lower_bound) { vg_lower = vg_lower_bound; }

  bool found_intersect = false;
  QPair<double, double> upper_intersect = loadLineIntersect(
    vg_upper,
    &found_intersect
  );
  if (!found_intersect) {
    emit calculatorMessage(
      QString("Failed to find an input intersect for vg=")
        + QString::number(vg_upper)
        + " V"
    );
    return;
  }
  found_intersect = false;
  QPair<double, double> lower_intersect = loadLineIntersect(
    vg_lower,
    &found_intersect
  );
  if (!found_intersect) {
    emit calculatorMessage(
      QString("Failed to find an input intersect for vg=")
        + QString::number(vg_lower)
        + " V"
    );
    return;
  }
  m_calculated_output_min_va = upper_intersect.first;
  m_calculated_output_max_ia = upper_intersect.second;
  m_calculated_output_max_va = lower_intersect.first;
  m_calculated_output_min_ia = lower_intersect.second;
  emit plotIORange(
    {m_calculated_output_min_va, m_calculated_output_max_va},
    {m_calculated_output_max_ia, m_calculated_output_min_ia}
  );
  #ifdef DEBUG_BUILD
  qDebug() << "[Calculator]{calculateIORange} End calculation: "
           << "va=[" << m_calculated_output_min_va << ", "
           << m_calculated_output_max_va << "], ia=["
           << m_calculated_output_min_ia << ", "
           << m_calculated_output_max_ia << "]";
  #endif // DEBUG_BUILD
  emit calculatorMessage(
    QString("Calculated IO range: va=[")
      + QString::number(m_calculated_output_min_va)
      + ", "
      + QString::number(m_calculated_output_max_va)
      + "], ia=["
      + QString::number(m_calculated_output_min_ia)
      + ", "
      + QString::number(m_calculated_output_max_ia)
      + "]"
  );
  if (m_mode == Mode::Reactive) {
    const double approx_output_power = (
      (m_plate_supply - m_calculated_output_min_va)
        * (m_calculated_output_max_ia * 0.001) 
        * 0.5
    );
    emit approximatedOutputPower(approx_output_power);
  }
}

bool Calculator::canCalculateLoadLine() const noexcept {
  return (
    m_load_ohms_valid 
      && m_plate_supply_valid
      && (
        (m_mode == Mode::Reactive)
          ? (m_bias_valid)
          : (true)
      )
  );
}

bool Calculator::canCalculateBiasIntersection() const noexcept {
  return (
    canCalculateLoadLine() && m_bias_valid
  );
}

bool Calculator::canCalculateIORange() const noexcept {
  return (
    canCalculateBiasIntersection() && m_input_vpp_valid
  );
}

QPair<double, double> Calculator::loadLineIntersect(
  double vg,
  bool* intersect_found
) const {
  QPair<double, double> ret(0., 0.);
  const QPair<int, int> bounding_indices = find_bounding_grid_curves(vg);
  InterpolatedCurve grid_curve;
  if ((bounding_indices.first == -1) || (bounding_indices.second == -1)) {
    if (intersect_found != nullptr) {
      (*intersect_found) = false;
    }
    return ret;
  } else if (bounding_indices.first != bounding_indices.second) {
    const InterpolatedCurve& first_curve = m_interp_curves[bounding_indices.first];
    const InterpolatedCurve& second_curve = m_interp_curves[bounding_indices.second];
    grid_curve = first_curve.interpolateWith(second_curve, vg);
  } else {
    grid_curve = m_interp_curves[bounding_indices.first];
  }
  for (int i = 0 ; i < grid_curve.sections.size() ; i ++) {
    InterpolatedCurve::Section& section = grid_curve.sections[i];
    if (
      is_linear_intersect(
        section.lower_bound, section.upper_bound,
        section.slope, section.intercept,
        m_calculated_ll_slope, m_calculated_ll_intercept
      )
    ) {
      ret = linear_intersect(
        section.slope, section.intercept,
        m_calculated_ll_slope, m_calculated_ll_intercept
      );
      if (intersect_found != nullptr) {
        (*intersect_found) = true;
      }
      return ret;
    }
  }
  if (intersect_found != nullptr) {
    (*intersect_found) = false;
  }
  return ret;
}

double Calculator::verticalIntersect(
  double vg,
  bool* intersect_found
) const {
  double ret = 0.0;
  const QPair<int, int> bounding_indices = find_bounding_grid_curves(vg);
  InterpolatedCurve grid_curve;
  if ((bounding_indices.first == -1) || (bounding_indices.second == -1)) {
    if (intersect_found != nullptr) {
      (*intersect_found) = false;
    }
    return ret;
  } else if (bounding_indices.first != bounding_indices.second) {
    const InterpolatedCurve& first_curve = m_interp_curves[bounding_indices.first];
    const InterpolatedCurve& second_curve = m_interp_curves[bounding_indices.second];
    grid_curve = first_curve.interpolateWith(second_curve, vg);
  } else {
    grid_curve = m_interp_curves[bounding_indices.first];
  }
  ret = grid_curve.f(m_plate_supply);
  if (intersect_found != nullptr) {
    (*intersect_found) = true;
  }
  return ret;
}
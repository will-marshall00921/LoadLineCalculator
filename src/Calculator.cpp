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
  m_calculated_bias_va = 0.0;
  m_calculated_bias_ia = 0.0;
  #ifdef DEBUG_BUILD
  qDebug() << "[Calculator]{clearCurves} Loaded and calculated variables cleared!";
  #endif // DEBUG_BUILD
}

double Calculator::setLoadValue(double r) {
  double retval = m_load_ohms;
  m_load_ohms = r;
  #ifdef DEBUG_BUILD
  qDebug() << "[Calculator]{setLoadValue} New load value (ohms): " << r;
  #endif // DEBUG_BUILD
  if (!m_load_ohms_valid) { m_load_ohms_valid = true; }
  if ((retval != r) && (canCalculateLoadLine())) {
    calculateLoadLine();
  }
  if (canCalculateBiasIntersection()) {
    calculateBiasIntersection();
  }
  return retval;
}

double Calculator::setPlateSupplyVoltage(int v) {
  double retval = m_plate_supply;
  m_plate_supply = static_cast<double>(v);
  #ifdef DEBUG_BUILD
  qDebug() << "[Calculator]{setPlateSupplyVoltage} New plate supply voltage: "
           << v;
  #endif // DEBUG_BUILD
  if (!m_plate_supply_valid) { m_plate_supply_valid = true; }
  if ((retval != m_plate_supply) && (canCalculateLoadLine())) {
    calculateLoadLine();
  }
  if (canCalculateBiasIntersection()) {
    calculateBiasIntersection();
  }
  return retval;
}

double Calculator::setBiasVoltage(double v) {
  double retval = m_bias;
  m_bias = v;
  #ifdef DEBUG_BUILD
  qDebug() << "[Calculator]{setBiasVoltage} New bias voltage: " << v;
  #endif // DEBUG_BUILD
  if (!m_bias_valid) { m_bias_valid = true; }
  if ((retval != v) && (canCalculateBiasIntersection())) {
    calculateBiasIntersection();
  }
  return retval;
}

Calculator::Mode Calculator::setMode(Calculator::Mode mode) {
  Calculator::Mode retval = m_mode;
  m_mode = mode;
  #ifdef DEBUG_BUILD
  qDebug() << "[Calculator]{setMode} New mode: " << static_cast<int>(mode);
  #endif // DEBUG_BUILD
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

QPair<int, int> Calculator::find_bounding_grid_curves(double tgt_vg) {
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
  voltages.append(0.);
  voltages.append(m_plate_supply);
  const double max_ia_ma = (1E3 * m_plate_supply / m_load_ohms);
  currents.append(1E3 * m_plate_supply / m_load_ohms);
  currents.append(0.);
  m_calculated_ll_slope = (-1. * max_ia_ma / m_plate_supply);
  m_calculated_ll_intercept = max_ia_ma;
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
  QPair<int, int> bounding_indices = find_bounding_grid_curves(m_bias);
  #ifdef DEBUG_BUILD
  qDebug() << "[Calculator]{calculateBiasIntersection} Bias vg=" << m_bias
           << " bounded by curve indices: (" << bounding_indices.first
           << ", " << bounding_indices.second << ")";
  #endif // DEBUG_BUILD
  InterpolatedCurve grid_curve;
  if ((bounding_indices.first == -1) || (bounding_indices.second == -1)) {
    emit calculatorMessage("Grid voltage out of bounds!");
    return; 
  } else if (bounding_indices.first != bounding_indices.second) {
    // interpolate the grid curve, then perform intersection
    InterpolatedCurve& first_curve = m_interp_curves[bounding_indices.first];
    InterpolatedCurve& second_curve = m_interp_curves[bounding_indices.second];
    grid_curve = first_curve.interpolateWith(second_curve, m_bias);
  } else {
    // exact match, perform single intersection
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
      QPair<double, double> intersection_point = linear_intersect(
        section.slope, section.intercept,
        m_calculated_ll_slope, m_calculated_ll_intercept
      );
      m_calculated_bias_va = intersection_point.first;
      m_calculated_bias_ia = intersection_point.second;
      #ifdef DEBUG_BUILD
      qDebug() << "[Calculator]{calculateBiasIntersection} End calculation: ("
               << m_calculated_bias_va << ", " << m_calculated_bias_ia << ")";
      #endif // DEBUG_BUILD
      emit plotBiasPoint(m_calculated_bias_va, m_calculated_bias_ia);
      emit calculatorMessage(
        QString("Plotting bias point, Vg=")
          + QString::number(m_bias)
          + " V: Va="
          + QString::number(m_calculated_bias_va)
          + " V, Ia="
          + QString::number(m_calculated_bias_ia)
      );
      return;
    }
  }
  emit calculatorMessage(
    "Failed to find an intersection with a grid curve and the load line!"
  );
}

bool Calculator::canCalculateLoadLine() const noexcept {
  return (
    m_load_ohms_valid && m_plate_supply_valid
  );
}

bool Calculator::canCalculateBiasIntersection() const noexcept {
  return (
    canCalculateLoadLine() && m_bias_valid
  );
}
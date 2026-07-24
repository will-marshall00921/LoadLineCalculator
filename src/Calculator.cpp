// LoadLineCalculator -- Calculator.cpp
// Author: Will Marshall
// Created: 7/23/2026, 6:12:12 PM

#include "Calculator.hpp"

// TODO: Figure out how to invalidate the initial parameters so that it can be reset without a plot

Calculator::Calculator()
  : QObject { nullptr }
  , m_thread { new QThread }
  , m_load_ohms { 0.0 }
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
}

double Calculator::setLoadValue(double r) {
  double retval = m_load_ohms;
  m_load_ohms = r;
  if (retval != r) {
    calculateLoadLine();
  }
  return retval;
}

double Calculator::setPlateSupplyVoltage(int v) {
  double retval = m_plate_supply;
  m_plate_supply = static_cast<double>(v);
  if (retval != m_plate_supply) {
    calculateLoadLine();
  }
  return retval;
}

Calculator::Mode Calculator::setMode(Calculator::Mode mode) {
  Calculator::Mode retval = m_mode;
  m_mode = mode;
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
    if (tgt_vg == m_interp_curves[i].vg) {
      retpair.first = i;
      retpair.second = i;
      break;
    } else if (
      (tgt_vg >= m_interp_curves[i].vg)
        && (tgt_vg <= m_interp_curves[i+1].vg)
    ) {
      retpair.first = i;
      retpair.second = (i+1);
      break;
    }
  }
  if (tgt_vg == m_interp_curves.back().vg) {
    retpair.first = (m_interp_curves.size()-1);
    retpair.second = (m_interp_curves.size()-1);
  }
  return retpair;
}

void Calculator::calculateLoadLine() {
  QVector<double> voltages;
  QVector<double> currents;
  voltages.append(0.);
  voltages.append(m_plate_supply);
  currents.append(1E3 * m_plate_supply / m_load_ohms);
  currents.append(0.);
  emit plotLoadLine(voltages, currents);
  emit calculatorMessage(
    QString("Plotting load line for Va=")
      + QString::number(m_plate_supply)
      + " V, Rl="
      + QString::number(m_load_ohms)
      + QString::fromStdWString(L" Ω")
  );
}
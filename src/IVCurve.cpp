// LoadLineCalculator -- IVCurve.cpp
// Author: Will Marshall
// Created: 7/19/2026, 6:25:38 PM

#include "IVCurve.hpp"
#include <utility>
#include <QtCore/QJsonValue>
#include <QtCore/QString>
#include <QtCore/QJsonArray>

IVCurve::IVCurve() noexcept
  : valid { false }
  , grid_voltage { 0.0 }
  , point_count { 0 }
  {}

IVCurve::IVCurve(const IVCurve& other)
  : valid { other.valid }
  , grid_voltage { other.grid_voltage }
  , point_count { other.point_count }
  , plate_voltages { other.plate_voltages }
  , plate_currents { other.plate_currents }
  {}

IVCurve::IVCurve(IVCurve&& other) noexcept
  : valid { std::move(other.valid) }
  , grid_voltage { std::move(other.grid_voltage) }
  , point_count { std::move(other.point_count) }
  , plate_voltages { std::move(other.plate_voltages) }
  , plate_currents { std::move(other.plate_currents) }
  {}

IVCurve::~IVCurve() noexcept {}

IVCurve& IVCurve::operator = (const IVCurve& other) {
  valid = other.valid;
  grid_voltage = other.grid_voltage;
  point_count = other.point_count;
  plate_voltages = other.plate_voltages;
  plate_currents = other.plate_currents;
  return (*this);
}

IVCurve& IVCurve::operator = (IVCurve&& other) noexcept {
  if ((&other) == this) { return (*this); }
  valid = std::move(other.valid);
  grid_voltage = std::move(other.grid_voltage);
  point_count = std::move(other.point_count);
  plate_voltages = std::move(other.plate_voltages);
  plate_currents = std::move(other.plate_currents);
  return (*this);
}

IVCurve IVCurve::fromJson(const QJsonObject& json) {
  IVCurve ret;
  bool grid_voltage_read = false;
  bool point_count_read = false;
  bool plate_voltages_read = false;
  bool plate_currents_read = false;
  QJsonValue grid_voltage_value = json.value("grid_voltage");
  if (grid_voltage_value.isDouble()) {
    grid_voltage_read = true;
    ret.grid_voltage = grid_voltage_value.toDouble();
  }
  QJsonValue point_count_value = json.value("point_count");
  if (point_count_value.isString()) {
    point_count_read = true;
    ret.point_count = point_count_value.toString().toInt();
  }
  QJsonValue plate_voltages_value = json.value("plate_voltages");
  if (plate_voltages_value.isArray()) {
    plate_voltages_read = true;
    QJsonArray plate_voltage_array = plate_voltages_value.toArray();
    for (const QJsonValue& plate_voltage_value : plate_voltage_array) {
      if (plate_voltage_value.isDouble()) {
        ret.plate_voltages.append(plate_voltage_value.toDouble());
      }
    }
  }
  QJsonValue plate_currents_value = json.value("plate_currents");
  if (plate_currents_value.isArray()) {
    plate_currents_read = true;
    QJsonArray plate_currents_array = plate_currents_value.toArray();
    for (const QJsonValue& plate_current_value : plate_currents_array) {
      if (plate_current_value.isDouble()) {
        ret.plate_currents.append(plate_current_value.toDouble());
      }
    }
  }
  ret.valid = (
    grid_voltage_read
      && point_count_read
      && (plate_voltages_read)
      && (plate_currents_read)
      && (ret.plate_voltages.size() == ret.point_count)
      && (ret.plate_currents.size() == ret.point_count)
  );
  return ret;
}
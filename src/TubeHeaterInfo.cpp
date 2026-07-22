// LoadLineCalculator -- TubeHeaterInfo.cpp
// Author: Will Marshall
// Created: 7/19/2026, 10:01:50 AM

#include "TubeManager.hpp"
#include <utility>

TubeManager::TubeHeaterInfo::TubeHeaterInfo() noexcept {}

TubeManager::TubeHeaterInfo::TubeHeaterInfo(const TubeManager::TubeHeaterInfo& other)
  : heater_voltage { other.heater_voltage }
  , heater_current { other.heater_current }
  {}

TubeManager::TubeHeaterInfo::TubeHeaterInfo(TubeManager::TubeHeaterInfo&& other) noexcept
  : heater_voltage { std::move(other.heater_voltage) }
  , heater_current { std::move(other.heater_current) }
  {}

TubeManager::TubeHeaterInfo::~TubeHeaterInfo() noexcept {}

TubeManager::TubeHeaterInfo& TubeManager::TubeHeaterInfo::operator = (const TubeManager::TubeHeaterInfo& other) {
  this->heater_voltage = other.heater_voltage;
  this->heater_current = other.heater_current;
  return (*this);
}

TubeManager::TubeHeaterInfo& TubeManager::TubeHeaterInfo::operator = (TubeManager::TubeHeaterInfo&& other) noexcept {
  if ((&other) == this) { return (*this); }
  this->heater_voltage = std::move(other.heater_voltage);
  this->heater_current = std::move(other.heater_current);
  return (*this);
}
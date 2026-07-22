// LoadLineCalculator -- TubeRatings.cpp
// Author: Will Marshall
// Created: 7/19/2026, 10:15:15 AM

#include "TubeManager.hpp"
#include <utility>

TubeManager::TubeRatings::TubeRatings() noexcept {}

TubeManager::TubeRatings::TubeRatings(const TubeManager::TubeRatings& other)
  : max_plate_voltage { other.max_plate_voltage }
  , max_screen_voltage { other.max_screen_voltage }
  , max_grid_voltage { other.max_grid_voltage }
  , min_grid_voltage { other.min_grid_voltage }
  , max_plate_power { other.max_plate_power }
  , max_screen_power { other.max_screen_power }
  , max_heater_cathode_voltage { other.max_heater_cathode_voltage }
  , min_heater_cathode_voltage { other.min_heater_cathode_voltage }
  {}

TubeManager::TubeRatings::TubeRatings(TubeManager::TubeRatings&& other) noexcept
  : max_plate_voltage { std::move(other.max_plate_voltage) }
  , max_screen_voltage { std::move(other.max_screen_voltage) }
  , max_grid_voltage { std::move(other.max_grid_voltage) }
  , min_grid_voltage { std::move(other.min_grid_voltage) }
  , max_plate_power { std::move(other.max_plate_power) }
  , max_screen_power { std::move(other.max_screen_power) }
  , max_heater_cathode_voltage { std::move(other.max_heater_cathode_voltage) }
  , min_heater_cathode_voltage { std::move(other.min_heater_cathode_voltage) }
  {}

TubeManager::TubeRatings::~TubeRatings() noexcept {}

TubeManager::TubeRatings& TubeManager::TubeRatings::operator = (
  const TubeManager::TubeRatings& other
) {
  this->max_plate_voltage = other.max_plate_voltage;
  this->max_screen_voltage = other.max_screen_voltage;
  this->max_grid_voltage = other.max_grid_voltage;
  this->min_grid_voltage = other.min_grid_voltage;
  this->max_plate_power = other.max_plate_power;
  this->max_screen_power = other.max_screen_power;
  this->max_heater_cathode_voltage = other.max_heater_cathode_voltage;
  this->min_heater_cathode_voltage = other.min_heater_cathode_voltage;
  return (*this);
}

TubeManager::TubeRatings& TubeManager::TubeRatings::operator = (
  TubeManager::TubeRatings&& other
) noexcept {
  if ((&other) == this) { return (*this); }
  this->max_plate_voltage = std::move(other.max_plate_voltage);
  this->max_screen_voltage = std::move(other.max_screen_voltage);
  this->max_grid_voltage = std::move(other.max_grid_voltage);
  this->min_grid_voltage = std::move(other.min_grid_voltage);
  this->max_plate_power = std::move(other.max_plate_power);
  this->max_screen_power = std::move(other.max_screen_power);
  this->max_heater_cathode_voltage = std::move(other.max_heater_cathode_voltage);
  this->min_heater_cathode_voltage = std::move(other.min_heater_cathode_voltage);
  return (*this);
}
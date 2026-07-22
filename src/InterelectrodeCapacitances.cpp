// LoadLineCalculator -- InterelectrodeCapacitances.cpp
// Author: Will Marshall
// Created: 7/19/2026, 10:07:47 AM

#include "TubeManager.hpp"
#include <utility>

TubeManager::InterelectrodeCapacitances::InterelectrodeCapacitances() noexcept {}

TubeManager::InterelectrodeCapacitances::InterelectrodeCapacitances(
  const TubeManager::InterelectrodeCapacitances& other
) : grid_to_plate { other.grid_to_plate }
  , grid_to_cathode { other.grid_to_cathode }
  , plate_to_cathode { other.plate_to_cathode }
  {}

TubeManager::InterelectrodeCapacitances::InterelectrodeCapacitances(
  TubeManager::InterelectrodeCapacitances&& other
) noexcept
  : grid_to_plate { std::move(other.grid_to_plate) }
  , grid_to_cathode { std::move(other.grid_to_cathode) }
  , plate_to_cathode { std::move(other.plate_to_cathode) }
  {}

TubeManager::InterelectrodeCapacitances::~InterelectrodeCapacitances() noexcept {}

TubeManager::InterelectrodeCapacitances& TubeManager::InterelectrodeCapacitances::operator = (
  const TubeManager::InterelectrodeCapacitances& other
) {
  this->grid_to_plate = other.grid_to_plate;
  this->grid_to_cathode = other.grid_to_cathode;
  this->plate_to_cathode = other.plate_to_cathode;
  return (*this);
}

TubeManager::InterelectrodeCapacitances& TubeManager::InterelectrodeCapacitances::operator = (
  TubeManager::InterelectrodeCapacitances&& other
) noexcept {
  if ((&other) == this) { return (*this); }
  this->grid_to_plate = std::move(other.grid_to_plate);
  this->grid_to_cathode = std::move(other.grid_to_cathode);
  this->plate_to_cathode = std::move(other.plate_to_cathode);
  return (*this);
}
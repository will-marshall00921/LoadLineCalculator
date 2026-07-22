// LoadLineCalculator -- TubeInfoEntry.cpp
// Author: Will Marshall
// Created: 7/19/2026, 9:55:25 AM

#include "TubeManager.hpp"
#include <utility>

TubeManager::TubeInfoEntry::TubeInfoEntry() noexcept
  : valid { false }
  , value { 0.0 }
  , units { "" }
  {}

TubeManager::TubeInfoEntry::TubeInfoEntry(double val, QString unts)
  : valid { true }
  , value { val }
  , units { unts }
  {}

TubeManager::TubeInfoEntry::TubeInfoEntry(const TubeManager::TubeInfoEntry& other)
  : valid { other.valid }
  , value { other.value }
  , units { other.units }
  {}

TubeManager::TubeInfoEntry::TubeInfoEntry(TubeManager::TubeInfoEntry&& other) noexcept
  : valid { std::move(other.valid) }
  , value { std::move(other.value) }
  , units { std::move(other.units) }
  {}

TubeManager::TubeInfoEntry::~TubeInfoEntry() noexcept {}

TubeManager::TubeInfoEntry& TubeManager::TubeInfoEntry::operator = (const TubeManager::TubeInfoEntry& other) {
  this->valid = other.valid;
  this->value = other.value;
  this->units = other.units;
  return (*this);
}

TubeManager::TubeInfoEntry& TubeManager::TubeInfoEntry::operator = (TubeManager::TubeInfoEntry&& other) noexcept {
  if ((&other) == this) { return (*this); }
  this->valid = std::move(other.valid);
  this->value = std::move(other.value);
  this->units = std::move(other.units);
  return (*this);
}

bool TubeManager::TubeInfoEntry::operator == (
  const TubeManager::TubeInfoEntry& other
) const noexcept {
  return (
    (this->valid == other.valid)
      && (this->value == other.value)
      && (this->units == other.units)
  );
}

bool TubeManager::TubeInfoEntry::operator != (
  const TubeManager::TubeInfoEntry& other
) const noexcept {
  return (
    (this->valid != other.valid)
      || (this->value != other.value)
      || (this->units != other.units)
  );
}
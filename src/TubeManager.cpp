// LoadLineCalculator -- TubeManager.cpp
// Author: Will Marshall
// Created: 7/19/2026, 10:25:25 AM

#include "TubeManager.hpp"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QJsonArray>
#include <QtCore/QFile>
#include <QtCore/QByteArray>

TubeManager::TubeManager()
  : m_thread { new QThread }
  , m_type_str { "--" }
  {
  this->moveToThread(m_thread);
  m_thread->start();
}

TubeManager::~TubeManager() noexcept {
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

void TubeManager::clearTubeInfo() {
  TubeInfoEntry blankEntry;
  if (checkClearEntry(this->m_heater_info.heater_voltage, blankEntry)) {
    emit heaterVoltageChanged(this->m_heater_info.heater_voltage);
  }
  if (checkClearEntry(this->m_heater_info.heater_current, blankEntry)) {
    emit heaterCurrentChanged(this->m_heater_info.heater_current);
  }
  if (checkClearEntry(this->m_capacitances.grid_to_plate, blankEntry)) {
    emit gridToPlateCapacitanceChanged(this->m_capacitances.grid_to_plate);
  }
  if (checkClearEntry(this->m_capacitances.grid_to_cathode, blankEntry)) {
    emit gridToCathodeCapacitanceChanged(this->m_capacitances.grid_to_cathode);
  }
  if (checkClearEntry(this->m_capacitances.plate_to_cathode, blankEntry)) {
    emit plateToCathodeCapacitanceChanged(this->m_capacitances.plate_to_cathode);
  }
  if (checkClearEntry(this->m_ratings.max_plate_voltage, blankEntry)) {
    emit maxPlateVoltageChanged(this->m_ratings.max_plate_voltage);
  }
  if (checkClearEntry(this->m_ratings.max_screen_voltage, blankEntry)) {
    emit maxScreenVoltageChanged(this->m_ratings.max_screen_voltage);
  }
  if (checkClearEntry(this->m_ratings.max_grid_voltage, blankEntry)) {
    emit maxGridVoltageChanged(this->m_ratings.max_grid_voltage);
  }
  if (checkClearEntry(this->m_ratings.min_grid_voltage, blankEntry)) {
    emit minGridVoltageChanged(this->m_ratings.min_grid_voltage);
  }
  if (checkClearEntry(this->m_ratings.max_plate_power, blankEntry)) {
    emit maxPlatePowerDissipationChanged(this->m_ratings.max_plate_power);
  }
  if (checkClearEntry(this->m_ratings.max_screen_power, blankEntry)) {
    emit maxScreenPowerDissipationChanged(this->m_ratings.max_screen_power);
  }
  if (checkClearEntry(this->m_ratings.max_heater_cathode_voltage, blankEntry)) {
    emit maxHeaterToCathodeVoltageChanged(this->m_ratings.max_heater_cathode_voltage);
  }
  if (checkClearEntry(this->m_ratings.min_heater_cathode_voltage, blankEntry)) {
    emit minHeaterToCathodeVoltageChanged(this->m_ratings.min_heater_cathode_voltage);
  }
  emit infoCleared();
}

void TubeManager::loadTubeInfo(const QString& path) {
  clearTubeInfo();
  auto read_value_and_units = [](const QJsonObject& json, const QString& key) {
    QJsonValue key_value = json.value(key);
    TubeInfoEntry ret;
    if (key_value.isDouble()) {
      ret.valid = true;
      ret.value = key_value.toDouble();
    }
    QJsonValue key_units = json.value(key + "_units");
    if (key_units.isString()) {
      ret.units = key_units.toString();
    }
    return ret;
  };
  QFile input_file(path);
  if (!input_file.open(QIODeviceBase::OpenModeFlag::ReadOnly | QIODeviceBase::OpenModeFlag::Text)) {
    emit tubeManagerStatus(QString("Failed to open file: ") + path);
    return;
  }
  QByteArray file_data = input_file.readAll();
  input_file.close();
  QJsonParseError json_parse_error;
  QJsonDocument file_json = QJsonDocument::fromJson(file_data, &json_parse_error);
  if ((json_parse_error.error != QJsonParseError::NoError) || (file_json.isNull()) || (file_json.isEmpty())) {
    emit tubeManagerStatus(QString("Failed to parse JSON file, parse error, null object, or empty file."));
    return;
  }
  QJsonObject top_level_object = file_json.object();
  QJsonValue type_value = top_level_object.value("type");
  if (type_value.isString()) {
    setTubeType(type_value.toString());
  }
  QJsonValue heater_info = top_level_object.value("heater");
  if (heater_info.isObject()) {
    QJsonObject heater_info_object = heater_info.toObject();
    setHeaterVoltage(read_value_and_units(heater_info_object, "vdc_parallel"));
    setHeaterCurrent(read_value_and_units(heater_info_object, "i_parallel"));
  }
  QJsonValue capacitances = top_level_object.value("capacitances");
  if (capacitances.isObject()) {
    QJsonObject capacitance_object = capacitances.toObject();
    setGridToPlateCapacitance(read_value_and_units(capacitance_object, "c_g1_a"));
    setGridToCathodeCapacitance(read_value_and_units(capacitance_object, "c_g1_k"));
    setPlateToCathodeCapacitance(read_value_and_units(capacitance_object, "c_a_k"));
  }
  QJsonValue ratings = top_level_object.value("ratings");
  if (ratings.isObject()) {
    QJsonObject ratings_object = ratings.toObject();
    setMaxPlateVoltage(read_value_and_units(ratings_object, "max_v_a_k"));
    setMaxScreenVoltage(read_value_and_units(ratings_object, "max_v_g2_k"));
    setMaxGridVoltage(read_value_and_units(ratings_object, "max_v_g1_k"));
    setMinGridVoltage(read_value_and_units(ratings_object, "min_v_g1_k"));
    setMaxPlatePowerDissipation(read_value_and_units(ratings_object, "max_p_a"));
    setMaxScreenPowerDissipation(read_value_and_units(ratings_object, "max_p_g2"));
    setMaxHeaterToCathodeVoltage(read_value_and_units(ratings_object, "max_v_k"));
    setMinHeaterToCathodeVoltage(read_value_and_units(ratings_object, "min_v_k"));
  }
  QJsonValue curves_value = top_level_object.value("curves");
  if (curves_value.isObject()) {
    #ifdef DEBUG_BUILD
    qDebug() << "Located curves object!";
    #endif // DEBUG_BUILD
    QJsonObject curves_top_object = curves_value.toObject();
    QJsonValue curves_data_value = curves_top_object.value("data");
    if (curves_data_value.isArray()) {
      #ifdef DEBUG_BUILD
      qDebug() << "Located curves data!";
      #endif // DEBUG_BUILD
      QJsonArray curves_array = curves_data_value.toArray();
      for (const QJsonValue& curve_value : curves_array) {
        if (curve_value.isObject()) {
          #ifdef DEBUG_BUILD
          qDebug() << "Located curve object!";
          #endif // DEBUG_BUILD
          QJsonObject curve_object = curve_value.toObject();
          IVCurve curve = IVCurve::fromJson(curve_object);
          if (curve.valid) {
            #ifdef DEBUG_BUILD
            qDebug() << "...valid curve!";
            #endif // DEBUG_BUILD
            emit curveLoaded(curve);
          }
        }
      }
      emit allCurvesLoaded();
    }
  }
  emit tubeManagerStatus(QString("Read file: ") + path);
}

TubeManager::TubeInfoEntry TubeManager::setHeaterVoltage(const TubeInfoEntry& x) {
  TubeInfoEntry ret = m_heater_info.heater_voltage;
  m_heater_info.heater_voltage = x;
  if (x != ret) {
    emit heaterVoltageChanged(m_heater_info.heater_voltage);
  }
  return ret;
}

TubeManager::TubeInfoEntry TubeManager::setHeaterCurrent(const TubeInfoEntry& x) {
  TubeInfoEntry ret = m_heater_info.heater_current;
  m_heater_info.heater_current = x;
  if (x != ret) {
    emit heaterCurrentChanged(m_heater_info.heater_current);
  }
  return ret;
}

TubeManager::TubeInfoEntry TubeManager::setGridToPlateCapacitance(const TubeInfoEntry& x) {
  TubeInfoEntry ret = m_capacitances.grid_to_plate;
  m_capacitances.grid_to_plate = x;
  if (x != ret) {
    emit gridToPlateCapacitanceChanged(m_capacitances.grid_to_plate);
  }
  return ret;
}

TubeManager::TubeInfoEntry TubeManager::setGridToCathodeCapacitance(const TubeInfoEntry& x) {
  TubeInfoEntry ret = m_capacitances.grid_to_cathode;
  m_capacitances.grid_to_cathode = x;
  if (x != ret) {
    emit gridToCathodeCapacitanceChanged(m_capacitances.grid_to_cathode);
  }
  return ret;
}

TubeManager::TubeInfoEntry TubeManager::setPlateToCathodeCapacitance(const TubeInfoEntry& x) {
  TubeInfoEntry ret = m_capacitances.plate_to_cathode;
  m_capacitances.plate_to_cathode = x;
  if (x != ret) {
    emit plateToCathodeCapacitanceChanged(m_capacitances.plate_to_cathode);
  }
  return ret;
}

TubeManager::TubeInfoEntry TubeManager::setMaxPlateVoltage(const TubeInfoEntry& x) {
  TubeInfoEntry ret = m_ratings.max_plate_voltage;
  m_ratings.max_plate_voltage = x;
  if (x != ret) {
    emit maxPlateVoltageChanged(m_ratings.max_plate_voltage);
  }
  return ret;
}

TubeManager::TubeInfoEntry TubeManager::setMaxScreenVoltage(const TubeInfoEntry& x) {
  TubeInfoEntry ret = m_ratings.max_screen_voltage;
  m_ratings.max_screen_voltage = x;
  if (x != ret) {
    emit maxScreenVoltageChanged(m_ratings.max_screen_voltage);
  }
  return ret;
}

TubeManager::TubeInfoEntry TubeManager::setMaxGridVoltage(const TubeInfoEntry& x) {
  TubeInfoEntry ret = m_ratings.max_grid_voltage;
  m_ratings.max_grid_voltage = x;
  if (x != ret) {
    emit maxGridVoltageChanged(m_ratings.max_grid_voltage);
  }
  return ret;
}

TubeManager::TubeInfoEntry TubeManager::setMinGridVoltage(const TubeInfoEntry& x) {
  TubeInfoEntry ret = m_ratings.min_grid_voltage;
  m_ratings.min_grid_voltage = x;
  if (x != ret) {
    emit minGridVoltageChanged(m_ratings.min_grid_voltage);
  }
  return ret;
}

TubeManager::TubeInfoEntry TubeManager::setMaxPlatePowerDissipation(const TubeInfoEntry& x) {
  TubeInfoEntry ret = m_ratings.max_plate_power;
  m_ratings.max_plate_power = x;
  if (x != ret) {
    emit maxPlatePowerDissipationChanged(m_ratings.max_plate_power);
  }
  return ret;
}

TubeManager::TubeInfoEntry TubeManager::setMaxScreenPowerDissipation(const TubeInfoEntry& x) {
  TubeInfoEntry ret = m_ratings.max_screen_power;
  m_ratings.max_screen_power = x;
  if (x != ret) {
    emit maxScreenPowerDissipationChanged(m_ratings.max_screen_power);
  }
  return ret;
}

TubeManager::TubeInfoEntry TubeManager::setMaxHeaterToCathodeVoltage(const TubeInfoEntry& x) {
  TubeInfoEntry ret = m_ratings.max_heater_cathode_voltage;
  m_ratings.max_heater_cathode_voltage = x;
  if (x != ret) {
    emit maxHeaterToCathodeVoltageChanged(m_ratings.max_heater_cathode_voltage);
  }
  return ret;
}

TubeManager::TubeInfoEntry TubeManager::setMinHeaterToCathodeVoltage(const TubeInfoEntry& x) {
  TubeInfoEntry ret = m_ratings.min_heater_cathode_voltage;
  m_ratings.min_heater_cathode_voltage = x;
  if (x != ret) {
    emit minHeaterToCathodeVoltageChanged(m_ratings.min_heater_cathode_voltage);
  }
  return ret;
}

QString TubeManager::setTubeType(const QString& tp) {
  QString retstr = m_type_str;
  m_type_str = tp;
  if (tp != retstr) {
    emit tubeTypeChanged(m_type_str);
  }
  return retstr;
}

bool TubeManager::checkClearEntry(TubeManager::TubeInfoEntry& ti, const TubeManager::TubeInfoEntry& ce) {
  bool retval = ti.valid;
  if (retval) {
    ti = ce;
  }
  return retval;
}
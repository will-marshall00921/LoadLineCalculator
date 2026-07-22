// LoadLineCalculator -- TubeManager.hpp
// Author: Will Marshall
// Created: 7/19/2026, 9:20:57 AM

#ifndef TUBEMANAGER_HPP
#define TUBEMANAGER_HPP

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QPair>
#include <QtCore/QString>
#include <QtCore/QList>
#include "IVCurve.hpp"

class TubeManager : public QObject {
  Q_OBJECT

  QThread* m_thread;
  QString m_type_str;

  public:
    struct TubeInfoEntry {
      bool valid;
      double value;
      QString units;

      TubeInfoEntry() noexcept;
      TubeInfoEntry(double value, QString units);
      TubeInfoEntry(const TubeInfoEntry& other);
      TubeInfoEntry(TubeInfoEntry&& other) noexcept;

      ~TubeInfoEntry() noexcept;

      TubeInfoEntry& operator = (const TubeInfoEntry& other);
      TubeInfoEntry& operator = (TubeInfoEntry&& other) noexcept;

      bool operator == (const TubeInfoEntry& other) const noexcept;
      bool operator != (const TubeInfoEntry& other) const noexcept;
    };

    struct TubeHeaterInfo {
      TubeInfoEntry heater_voltage;
      TubeInfoEntry heater_current;

      TubeHeaterInfo() noexcept;
      TubeHeaterInfo(const TubeHeaterInfo& other);
      TubeHeaterInfo(TubeHeaterInfo&& other) noexcept;

      ~TubeHeaterInfo() noexcept;

      TubeHeaterInfo& operator = (const TubeHeaterInfo& other);
      TubeHeaterInfo& operator = (TubeHeaterInfo&& other) noexcept;
    };

    struct InterelectrodeCapacitances {
      TubeInfoEntry grid_to_plate;
      TubeInfoEntry grid_to_cathode;
      TubeInfoEntry plate_to_cathode;

      InterelectrodeCapacitances() noexcept;
      InterelectrodeCapacitances(const InterelectrodeCapacitances& other);
      InterelectrodeCapacitances(InterelectrodeCapacitances&& other) noexcept;

      ~InterelectrodeCapacitances() noexcept;

      InterelectrodeCapacitances& operator = (const InterelectrodeCapacitances& other);
      InterelectrodeCapacitances& operator = (InterelectrodeCapacitances&& other) noexcept;
    };

    struct TubeRatings {
      TubeInfoEntry max_plate_voltage;
      TubeInfoEntry max_screen_voltage;
      TubeInfoEntry max_grid_voltage;
      TubeInfoEntry min_grid_voltage;
      TubeInfoEntry max_plate_power;
      TubeInfoEntry max_screen_power;
      TubeInfoEntry max_heater_cathode_voltage;
      TubeInfoEntry min_heater_cathode_voltage;

      TubeRatings() noexcept;
      TubeRatings(const TubeRatings& other);
      TubeRatings(TubeRatings&& other) noexcept;

      ~TubeRatings() noexcept;

      TubeRatings& operator = (const TubeRatings& other);
      TubeRatings& operator = (TubeRatings&& other) noexcept;
    };

    TubeManager();

    ~TubeManager() noexcept;

    Q_SIGNAL void heaterVoltageChanged(TubeInfoEntry x);
    Q_SIGNAL void heaterCurrentChanged(TubeInfoEntry x);
    Q_SIGNAL void gridToPlateCapacitanceChanged(TubeInfoEntry x);
    Q_SIGNAL void gridToCathodeCapacitanceChanged(TubeInfoEntry x);
    Q_SIGNAL void plateToCathodeCapacitanceChanged(TubeInfoEntry x);
    Q_SIGNAL void maxPlateVoltageChanged(TubeInfoEntry x);
    Q_SIGNAL void maxScreenVoltageChanged(TubeInfoEntry x);
    Q_SIGNAL void maxGridVoltageChanged(TubeInfoEntry x);
    Q_SIGNAL void minGridVoltageChanged(TubeInfoEntry x);
    Q_SIGNAL void maxPlatePowerDissipationChanged(TubeInfoEntry x);
    Q_SIGNAL void maxScreenPowerDissipationChanged(TubeInfoEntry x);
    Q_SIGNAL void maxHeaterToCathodeVoltageChanged(TubeInfoEntry x);
    Q_SIGNAL void minHeaterToCathodeVoltageChanged(TubeInfoEntry x);
    Q_SIGNAL void tubeManagerStatus(QString msg);
    Q_SIGNAL void tubeTypeChanged(QString tp);
    Q_SIGNAL void infoCleared();
    Q_SIGNAL void infoLoaded();
    Q_SIGNAL void curveLoaded(IVCurve curve);
    Q_SIGNAL void allCurvesLoaded();

    Q_SLOT void clearTubeInfo();
    Q_SLOT void loadTubeInfo(const QString& path);

    Q_SLOT TubeInfoEntry setHeaterVoltage(const TubeInfoEntry& x);
    Q_SLOT TubeInfoEntry setHeaterCurrent(const TubeInfoEntry& x);
    Q_SLOT TubeInfoEntry setGridToPlateCapacitance(const TubeInfoEntry& x);
    Q_SLOT TubeInfoEntry setGridToCathodeCapacitance(const TubeInfoEntry& x);
    Q_SLOT TubeInfoEntry setPlateToCathodeCapacitance(const TubeInfoEntry& x);
    Q_SLOT TubeInfoEntry setMaxPlateVoltage(const TubeInfoEntry& x);
    Q_SLOT TubeInfoEntry setMaxScreenVoltage(const TubeInfoEntry& x);
    Q_SLOT TubeInfoEntry setMaxGridVoltage(const TubeInfoEntry& x);
    Q_SLOT TubeInfoEntry setMinGridVoltage(const TubeInfoEntry& x);
    Q_SLOT TubeInfoEntry setMaxPlatePowerDissipation(const TubeInfoEntry& x);
    Q_SLOT TubeInfoEntry setMaxScreenPowerDissipation(const TubeInfoEntry& x);
    Q_SLOT TubeInfoEntry setMaxHeaterToCathodeVoltage(const TubeInfoEntry& x);
    Q_SLOT TubeInfoEntry setMinHeaterToCathodeVoltage(const TubeInfoEntry& x);

    Q_SLOT QString setTubeType(const QString& tp);

  private:
    TubeHeaterInfo m_heater_info;
    InterelectrodeCapacitances m_capacitances;
    TubeRatings m_ratings;

    static bool checkClearEntry(TubeInfoEntry& ti, const TubeInfoEntry& ce);


};

#endif // TUBEMANAGER_HPP
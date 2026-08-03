// LoadLineCalculator -- TubeDataDialog.cpp
// Author: Will Marshall
// Created: 8/2/2026, 5:47:26 PM

#include "TubeDataDialog.hpp"
#include "ui_tubedatadialog.h"
// #include "ui_tubedatadialog.temp.h"
#include <QtCore/QSignalBlocker>
#include <QtWidgets/QFileDialog>
#include <QtCore/QStandardPaths>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QByteArray>
#include <QtCore/QFile>

TubeDataDialog::TubeDataDialog(QWidget* parent)
  : QDialog { parent }
  , ui { new Ui::TubeDataDialog }
  , m_tube_type { 0 }
  , m_heater_voltage_v { 0. }
  , m_heater_current { 0. }
  , m_heater_current_units { "A" }
  , m_c_g_a_pf { 0. }
  , m_c_g_a_enabled { false }
  , m_c_g_k_pf { 0. }
  , m_c_g_k_enabled { false }
  , m_c_a_k_pf { 0. }
  , m_c_a_k_enabled { false }
  , m_max_plate_voltage_v { -1 }
  , m_max_screen_voltage_v { -1 }
  , m_max_screen_voltage_enabled { false }
  , m_max_grid_voltage_v { 0. }
  , m_max_grid_voltage_enabled { false }
  , m_min_grid_voltage_v { 0. }
  , m_min_grid_voltage_enabled { false }
  , m_max_plate_power_w { 0. }
  , m_max_plate_power_enabled { false }
  , m_max_screen_power_w { 0. }
  , m_max_screen_power_enabled { false }
  , m_max_heater_cathode_voltage_v { 0. }
  , m_max_heater_cathode_voltage_enabled { false }
  , m_min_heater_cathode_voltage_v { 0. }
  , m_min_heater_cathode_voltage_enabled { false }
  , m_csv_path_enabled { false }
  {
  ui->setupUi(this);

  m_tube_dir = (
    QStandardPaths::writableLocation(
      QStandardPaths::StandardLocation::AppDataLocation
    ) + "/tubes"
  );

  ui->heaterVoltageScalableSpinBox->setUnitsText("V");
  ui->heaterVoltageScalableSpinBox->setScaleIndex(2);
  ui->heaterCurrentScalableSpinBox->setUnitsText("A");
  ui->heaterCurrentScalableSpinBox->setScaleIndex(2);
  ui->gridToPlateCapacitanceScalableSpinBox->setUnitsText("F");
  ui->gridToPlateCapacitanceScalableSpinBox->setScaleIndex(0); // set to pF
  ui->gridToPlateCapacitanceScalableSpinBox->enablePlaceholder();
  ui->gridToCathodeCapacitanceScalableSpinBox->setUnitsText("F");
  ui->gridToCathodeCapacitanceScalableSpinBox->setScaleIndex(0); // set to pF
  ui->gridToCathodeCapacitanceScalableSpinBox->enablePlaceholder();
  ui->plateToCathodeCapacitanceScalableSpinBox->setUnitsText("F");
  ui->plateToCathodeCapacitanceScalableSpinBox->setScaleIndex(0); // set to pF
  ui->plateToCathodeCapacitanceScalableSpinBox->enablePlaceholder();
  QObject::connect(
    ui->nameLineEdit,
    &QLineEdit::textChanged,
    this,
    &TubeDataDialog::setTubeName
  );
  QObject::connect(
    ui->typeComboBox,
    &QComboBox::currentIndexChanged,
    this,
    &TubeDataDialog::setTubeType
  );
  QObject::connect(
    ui->heaterVoltageScalableSpinBox,
    &ScalableSpinBoxValue::scaledValueChanged,
    this,
    &TubeDataDialog::setHeaterVoltage
  );
  QObject::connect(
    ui->heaterCurrentScalableSpinBox,
    &ScalableSpinBoxValue::scaledValueChanged,
    this,
    &TubeDataDialog::setHeaterCurrent
  );
  QObject::connect(
    ui->heaterCurrentScalableSpinBox,
    &ScalableSpinBoxValue::scalingRatioChanged,
    this,
    &TubeDataDialog::setHeaterCurrentUnits
  );
  QObject::connect(
    ui->gridToPlateCapacitanceScalableSpinBox,
    &ScalableSpinBoxValue::scaledValueChanged,
    this,
    &TubeDataDialog::setGridToPlateCapacitance
  );
  QObject::connect(
    ui->gridToPlateCapacitanceCheckBox,
    &QCheckBox::toggled,
    this,
    &TubeDataDialog::setGridToPlateCapacitanceEnabled
  );
  QObject::connect(
    ui->gridToCathodeCapacitanceScalableSpinBox,
    &ScalableSpinBoxValue::scaledValueChanged,
    this,
    &TubeDataDialog::setGridToCathodeCapacitance
  );
  QObject::connect(
    ui->gridToCathodeCapacitanceCheckBox,
    &QCheckBox::toggled,
    this,
    &TubeDataDialog::setGridToCathodeCapacitanceEnabled
  );
  QObject::connect(
    ui->plateToCathodeCapacitanceScalableSpinBox,
    &ScalableSpinBoxValue::scaledValueChanged,
    this,
    &TubeDataDialog::setPlateToCathodeCapacitance
  );
  QObject::connect(
    ui->plateToCathodeCapacitanceCheckBox,
    &QCheckBox::toggled,
    this,
    &TubeDataDialog::setPlateToCathodeCapacitanceEnabled
  );
  QObject::connect(
    ui->maxPlateVoltageSpinBox,
    &QSpinBox::valueChanged,
    this,
    &TubeDataDialog::setMaxPlateVoltage
  );
  QObject::connect(
    ui->maxScreenVoltageSpinBox,
    &QSpinBox::valueChanged,
    this,
    &TubeDataDialog::setMaxScreenVoltage
  );
  QObject::connect(
    ui->maxScreenVoltageCheckBox,
    &QCheckBox::toggled,
    this,
    &TubeDataDialog::setMaxScreenVoltageEnabled
  );
  QObject::connect(
    ui->maxGridVoltageSpinBox,
    &QDoubleSpinBox::valueChanged,
    this,
    &TubeDataDialog::setMaxGridVoltage
  );
  QObject::connect(
    ui->maxGridVoltageCheckBox,
    &QCheckBox::toggled,
    this,
    &TubeDataDialog::setMaxGridVoltageEnabled
  );
  QObject::connect(
    ui->minGridVoltageSpinBox,
    &QDoubleSpinBox::valueChanged,
    this,
    &TubeDataDialog::setMinGridVoltage
  );
  QObject::connect(
    ui->minGridVoltageCheckBox,
    &QCheckBox::toggled,
    this,
    &TubeDataDialog::setMinGridVoltageEnabled
  );
  QObject::connect(
    ui->maxPlatePowerSpinBox,
    &QDoubleSpinBox::valueChanged,
    this,
    &TubeDataDialog::setMaxPlatePowerDissipation
  );
  QObject::connect(
    ui->maxPlatePowerCheckBox,
    &QCheckBox::toggled,
    this,
    &TubeDataDialog::setMaxPlatePowerDissipationEnabled
  );
  QObject::connect(
    ui->maxScreenPowerSpinBox,
    &QDoubleSpinBox::valueChanged,
    this,
    &TubeDataDialog::setMaxScreenPowerDissipation
  );
  QObject::connect(
    ui->maxScreenPowerCheckBox,
    &QCheckBox::toggled,
    this,
    &TubeDataDialog::setMaxScreenPowerDissipationEnabled
  );
  QObject::connect(
    ui->maxHeaterToCathodeVoltageSpinBox,
    &QSpinBox::valueChanged,
    this,
    &TubeDataDialog::setMaxHeaterToCathodeVoltage
  );
  QObject::connect(
    ui->maxHeaterToCathodeVoltageCheckBox,
    &QCheckBox::toggled,
    this,
    &TubeDataDialog::setMaxHeaterToCathodeVoltageEnabled
  );
  QObject::connect(
    ui->minHeaterToCathodeVoltageSpinBox,
    &QSpinBox::valueChanged,
    this,
    &TubeDataDialog::setMinHeaterToCathodeVoltage
  );
  QObject::connect(
    ui->minHeaterToCathodeVoltageCheckBox,
    &QCheckBox::toggled,
    this,
    &TubeDataDialog::setMinHeaterToCathodeVoltageEnabled
  );
  QObject::connect(
    ui->csvPathLineEdit,
    &QLineEdit::textChanged,
    this,
    &TubeDataDialog::setTransferCurveCSVPath
  );
  QObject::connect(
    ui->csvPathBrowseButton,
    &QToolButton::clicked,
    this,
    &TubeDataDialog::browseTransferCurveCSVPath
  );
  QObject::connect(
    ui->csvPathCheckBox,
    &QCheckBox::toggled,
    this,
    &TubeDataDialog::setTransferCurveCSVPathEnabled
  );
  // QObject::connect(
  //   ui->buttonBox,
  //   &QDialogButtonBox::accepted,
  //   this,
  //   [this]() {
  //     saveTubeFile();
  //     close();
  //   }
  // );
  // QObject::connect(
  //   ui->buttonBox,
  //   &QDialogButtonBox::rejected,
  //   this,
  //   &TubeDataDialog::close
  // );
}

TubeDataDialog::~TubeDataDialog() noexcept {
  if (ui != nullptr) {
    delete ui;
  }
}

void TubeDataDialog::setTubeDirectory(const QString& dir) {
  m_tube_dir = dir;
}

QString TubeDataDialog::setTubeName(const QString& name) {
  QString retstr = m_tube_name;
  m_tube_name = name;
  return retstr;
}

int TubeDataDialog::setTubeType(int type) {
  int retval = m_tube_type;
  m_tube_type = type;
  return retval;
}

double TubeDataDialog::setHeaterVoltage(double v) {
  double retval = m_heater_voltage_v;
  m_heater_voltage_v = v;
  return retval;
}

double TubeDataDialog::setHeaterCurrent(double i) {
  double retval = m_heater_current;
  m_heater_current = i;
  return retval;
}

void TubeDataDialog::setHeaterCurrentUnits(double scale) {
  if (scale == 1.0) {
    m_heater_current_units = "A";
  } else if (scale == 1E-3) {
    m_heater_current_units = "mA";
  } else if (scale == 1E3) {
    m_heater_current_units = "kA";
  } else if (scale == 1E-9) {
    m_heater_current_units = "pA";
  }
}

double TubeDataDialog::setGridToPlateCapacitance(double c) {
  double retval = m_c_g_a_pf;
  m_c_g_a_pf = (c * 1E9);
  return retval;
}

void toggleEnabled(
  QLabel* label,
  ScalableSpinBoxValue* scalableSpinBox,
  bool enabled
) {
  label->setEnabled(enabled);
  scalableSpinBox->setEnabled(enabled);
  if (!enabled) { scalableSpinBox->enablePlaceholder(); }
}

void toggleEnabled(
  QLabel* label,
  QSpinBox* spinBox,
  bool enabled
) {
  label->setEnabled(enabled);
  spinBox->setEnabled(enabled);
  if (!enabled) {
    QSignalBlocker spinBoxBlocker(spinBox);
    spinBox->setValue(
      spinBox->minimum()
    );
  }
}

void toggleEnabled(
  QLabel* label,
  QDoubleSpinBox* spinBox,
  bool enabled
) {
  label->setEnabled(enabled);
  spinBox->setEnabled(enabled);
  if (!enabled) {
    QSignalBlocker spinBoxBlocker(spinBox);
    spinBox->setValue(
      spinBox->minimum()
    );
  }
}

void TubeDataDialog::setGridToPlateCapacitanceEnabled(bool enabled) {
  toggleEnabled(
    ui->gridToPlateCapacitanceLabel,
    ui->gridToPlateCapacitanceScalableSpinBox,
    enabled
  );
  m_c_g_a_enabled = enabled;
}

double TubeDataDialog::setGridToCathodeCapacitance(double c) {
  double retval = m_c_g_k_pf;
  m_c_g_k_pf = (c * 1E9);
  return retval;
}

void TubeDataDialog::setGridToCathodeCapacitanceEnabled(bool enabled) {
  toggleEnabled(
    ui->gridToCathodeCapacitanceLabel,
    ui->gridToCathodeCapacitanceScalableSpinBox,
    enabled
  );
  m_c_g_k_enabled = enabled;
}

double TubeDataDialog::setPlateToCathodeCapacitance(double c) {
  double retval = m_c_a_k_pf;
  m_c_a_k_pf = (c * 1E9);
  return retval;
}

void TubeDataDialog::setPlateToCathodeCapacitanceEnabled(bool enabled) {
  toggleEnabled(
    ui->plateToCathodeCapacitanceLabel,
    ui->plateToCathodeCapacitanceScalableSpinBox,
    enabled
  );
  m_c_a_k_enabled = enabled;
}

int TubeDataDialog::setMaxPlateVoltage(int v) {
  int retval = m_max_plate_voltage_v;
  m_max_plate_voltage_v = v;
  return retval;
}

int TubeDataDialog::setMaxScreenVoltage(int v) {
  int retval = m_max_screen_voltage_v;
  m_max_screen_voltage_v = v;
  return retval;
}

void TubeDataDialog::setMaxScreenVoltageEnabled(bool enabled) {
  toggleEnabled(
    ui->maxScreenVoltageLabel,
    ui->maxScreenVoltageSpinBox,
    enabled
  );
  m_max_screen_voltage_enabled = enabled;
}

double TubeDataDialog::setMaxGridVoltage(double v) {
  double retval = m_max_grid_voltage_v;
  m_max_grid_voltage_v = v;
  return retval;
}

void TubeDataDialog::setMaxGridVoltageEnabled(bool enabled) {
  toggleEnabled(
    ui->maxGridVoltageLabel,
    ui->maxGridVoltageSpinBox,
    enabled
  );
  m_max_grid_voltage_enabled = enabled;
}

double TubeDataDialog::setMinGridVoltage(double v) {
  double retval = m_min_grid_voltage_v;
  m_min_grid_voltage_v = v;
  return retval;
}

void TubeDataDialog::setMinGridVoltageEnabled(bool enabled) {
  toggleEnabled(
    ui->minGridVoltageLabel,
    ui->minGridVoltageSpinBox,
    enabled
  );
  m_min_grid_voltage_enabled = enabled;
}

double TubeDataDialog::setMaxPlatePowerDissipation(double p) {
  double retval = m_max_plate_power_w;
  m_max_plate_power_w = p;
  return retval;
}

void TubeDataDialog::setMaxPlatePowerDissipationEnabled(bool enabled) {
  toggleEnabled(
    ui->maxPlatePowerLabel,
    ui->maxPlatePowerSpinBox,
    enabled
  );
  m_max_plate_power_enabled = enabled;
}

double TubeDataDialog::setMaxScreenPowerDissipation(double p) {
  double retval = m_max_screen_power_w;
  m_max_screen_power_w = p;
  return retval;
}

void TubeDataDialog::setMaxScreenPowerDissipationEnabled(bool enabled) {
  toggleEnabled(
    ui->maxScreenPowerLabel,
    ui->maxScreenPowerSpinBox,
    enabled
  );
  m_max_screen_power_enabled = enabled;
}

int TubeDataDialog::setMaxHeaterToCathodeVoltage(int v) {
  int retval = m_max_heater_cathode_voltage_v;
  m_max_heater_cathode_voltage_v = v;
  return retval;
}

void TubeDataDialog::setMaxHeaterToCathodeVoltageEnabled(bool enabled) {
  toggleEnabled(
    ui->maxHeaterToCathodeVoltageLabel,
    ui->maxHeaterToCathodeVoltageSpinBox,
    enabled
  );
  m_max_heater_cathode_voltage_enabled = enabled;
}

int TubeDataDialog::setMinHeaterToCathodeVoltage(int v) {
  int retval = m_min_heater_cathode_voltage_v;
  m_min_heater_cathode_voltage_v = v;
  return retval;
}

void TubeDataDialog::setMinHeaterToCathodeVoltageEnabled(bool enabled) {
  toggleEnabled(
    ui->minHeaterToCathodeVoltageLabel,
    ui->minHeaterToCathodeVoltageSpinBox,
    enabled
  );
  m_min_heater_cathode_voltage_enabled = enabled;
}

QString TubeDataDialog::setTransferCurveCSVPath(const QString& path) {
  QString retstr = m_csv_path;
  m_csv_path = path;
  return retstr;
}

void TubeDataDialog::browseTransferCurveCSVPath() {
  QString selected_path = QFileDialog::getOpenFileName(
    this,
    "Select CSV Data",
    QStandardPaths::writableLocation(
      QStandardPaths::StandardLocation::DocumentsLocation
    ),
    "CSV Files (*.csv)"
  );
  if (selected_path.isEmpty()) {
    return;
  }
  ui->csvPathLineEdit->setText(selected_path);
}

void TubeDataDialog::setTransferCurveCSVPathEnabled(bool enabled) {
  ui->csvPathBrowseButton->setEnabled(enabled);
  ui->csvPathLineEdit->setEnabled(enabled);
  ui->csvPathLabel->setEnabled(enabled);
  QSignalBlocker lineEditBlocker(ui->csvPathLineEdit);
  if (!enabled) {
    ui->csvPathLineEdit->clear();
  } else {
    ui->csvPathLineEdit->setText(m_csv_path);
  }
  m_csv_path_enabled = enabled;
}

void TubeDataDialog::saveTubeFile() {
  QJsonObject tube_obj;
  if (m_tube_name.isEmpty()) {
    return;
  }
  tube_obj.insert(
    "name",
    QJsonValue::fromVariant(m_tube_name)
  );
  tube_obj.insert(
    "type",
    QJsonValue::fromVariant(
      (m_tube_type == 0)
        ? (QString("Triode"))
        : (QString("Pentode"))
    )
  );
  QJsonObject heater_obj;
  heater_obj.insert(
    "vdc_parallel",
    QJsonValue::fromVariant(m_heater_voltage_v)
  );
  heater_obj.insert(
    "vdc_parallel_units",
    QJsonValue::fromVariant(QString("V"))
  );
  heater_obj.insert(
    "i_parallel",
    QJsonValue::fromVariant(m_heater_current)
  );
  heater_obj.insert(
    "i_parallel_units",
    QJsonValue::fromVariant(m_heater_current_units)
  );
  tube_obj.insert(
    "heater",
    QJsonValue::fromVariant(heater_obj)
  );
  if (
    m_c_g_a_enabled
      || m_c_g_k_enabled
      || m_c_a_k_enabled
  ) {
    QJsonObject cap_obj;
    if (m_c_g_a_enabled) {
      cap_obj.insert(
        "c_g1_a",
        QJsonValue::fromVariant(m_c_g_a_pf)
      );
      cap_obj.insert(
        "c_g1_a_units",
        QJsonValue::fromVariant(QString("pF"))
      );
    }
    if (m_c_g_k_enabled) {
      cap_obj.insert(
        "c_g1_k",
        QJsonValue::fromVariant(m_c_g_k_pf)
      );
      cap_obj.insert(
        "c_g1_k_units",
        QJsonValue::fromVariant(QString("pF"))
      );
    }
    if (m_c_a_k_enabled) {
      cap_obj.insert(
        "c_a_k",
        QJsonValue::fromVariant(m_c_a_k_pf)
      );
      cap_obj.insert(
        "c_a_k_units",
        QJsonValue::fromVariant(QString("pF"))
      );
    }
    tube_obj.insert(
      "capacitances",
      QJsonValue::fromVariant(cap_obj)
    );
  }
  QJsonObject ratings_obj;
  ratings_obj.insert(
    "max_v_a_k",
    QJsonValue::fromVariant(m_max_plate_voltage_v)
  );
  ratings_obj.insert(
    "max_v_a_k_units",
    QJsonValue::fromVariant(QString("V"))
  );
  if (m_max_screen_voltage_enabled) {
    ratings_obj.insert(
      "max_v_g2_k",
      QJsonValue::fromVariant(m_max_screen_voltage_v)
    );
    ratings_obj.insert(
      "max_v_g2_k_units",
      QJsonValue::fromVariant(QString("V"))
    );
  }
  if (m_max_grid_voltage_enabled) {
    ratings_obj.insert(
      "max_v_g1_k",
      QJsonValue::fromVariant(m_max_grid_voltage_v)
    );
    ratings_obj.insert(
      "max_v_g1_k_units",
      QJsonValue::fromVariant(QString("V"))
    );
  }
  if (m_min_grid_voltage_enabled) {
    ratings_obj.insert(
      "min_v_g1_k",
      QJsonValue::fromVariant(m_min_grid_voltage_v)
    );
    ratings_obj.insert(
      "min_v_g1_k_units",
      QJsonValue::fromVariant(QString("V"))
    );
  }
  if (m_max_plate_power_enabled) {
    ratings_obj.insert(
      "max_p_a",
      QJsonValue::fromVariant(m_max_plate_power_w)
    );
    ratings_obj.insert(
      "max_p_a_units",
      QJsonValue::fromVariant(QString("W"))
    );
  }
  if (m_max_screen_power_enabled) {
    ratings_obj.insert(
      "max_p_g2",
      QJsonValue::fromVariant(m_max_screen_power_w)
    );
    ratings_obj.insert(
      "max_p_g2_units",
      QJsonValue::fromVariant(QString("W"))
    );
  }
  if (m_max_heater_cathode_voltage_enabled) {
    ratings_obj.insert(
      "max_v_k",
      QJsonValue::fromVariant(m_max_heater_cathode_voltage_v)
    );
    ratings_obj.insert(
      "max_v_k_units",
      QJsonValue::fromVariant(QString("V"))
    );
  }
  if (m_min_heater_cathode_voltage_enabled) {
    ratings_obj.insert(
      "min_v_k",
      QJsonValue::fromVariant(m_min_heater_cathode_voltage_v)
    );
    ratings_obj.insert(
      "min_v_k_units",
      QJsonValue::fromVariant(QString("V"))
    );
  }
  tube_obj.insert(
    "ratings",
    QJsonValue::fromVariant(ratings_obj)
  );
  QJsonDocument tube_doc(tube_obj);
  QByteArray tube_data = tube_doc.toJson();
  QString tube_path = (m_tube_dir + "/" + m_tube_name + ".json");
  QFile tube_file(tube_path);
  if (!tube_file.open(QIODevice::WriteOnly)) {
    emit statusMessage(QString("Failed to open file: '") + tube_path + "'");
    return;
  }
  tube_file.write(tube_data);
  emit statusMessage(QString("Wrote tube file: '") + tube_path + "'");
}

void TubeDataDialog::accept() {
  saveTubeFile();
  QDialog::accept();
}
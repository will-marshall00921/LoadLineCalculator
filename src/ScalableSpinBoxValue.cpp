// LoadLineCalculator -- ScalableSpinBoxValue.cpp
// Author: Will Marshall
// Created: 7/18/2026, 3:07:23 PM

#include "ScalableSpinBoxValue.hpp"
#include <QtCore/QSignalBlocker>

ScalableSpinBoxValue::ScalableSpinBoxValue(QWidget* parent)
  : QWidget { parent }
  , m_horizontalLayout { nullptr }
  , spinBox { nullptr }
  , comboBox { nullptr }
  , m_scaled_value { 0. }
  , m_raw_value { 0 }
  , m_scaling { 0 }
  , m_units_text { "--" }
  {
  m_horizontalLayout = new QHBoxLayout(this);
  m_horizontalLayout->setObjectName("horizontalLayout");
  spinBox = new QSpinBox(this);
  spinBox->setObjectName("spinBox");
  QSizePolicy spinBoxSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);
  spinBoxSizePolicy.setHorizontalStretch(0);
  spinBoxSizePolicy.setVerticalStretch(0);
  spinBoxSizePolicy.setHeightForWidth(
    spinBox->sizePolicy().hasHeightForWidth()
  );
  spinBox->setSizePolicy(spinBoxSizePolicy);
  spinBox->setMinimumWidth(100);
  m_horizontalLayout->addWidget(spinBox);
  comboBox = new QComboBox(this);
  comboBox->setObjectName("comboBox");
  m_scales.append(1.E-3);
  m_prefixes.append('m');
  comboBox->addItem(QString("m")+m_units_text);
  m_scales.append(1.);
  m_prefixes.append(' ');
  comboBox->addItem(QString("")+m_units_text);
  m_scales.append(1.E3);
  m_prefixes.append('k');
  comboBox->addItem(QString("k")+m_units_text);
  m_scales.append(1.E6);
  m_prefixes.append('M');
  comboBox->addItem(QString("M")+m_units_text);
  QSizePolicy comboBoxSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Fixed);
  comboBoxSizePolicy.setHorizontalStretch(0);
  comboBoxSizePolicy.setVerticalStretch(0);
  comboBoxSizePolicy.setHeightForWidth(
    comboBox->sizePolicy().hasHeightForWidth()
  );
  comboBox->setSizePolicy(comboBoxSizePolicy);
  m_horizontalLayout->addWidget(comboBox);

  QObject::connect(
    spinBox,
    &QSpinBox::valueChanged,
    [this](int x) {
      this->updateValue(x, m_scaling);
    }
  );
  QObject::connect(
    comboBox,
    &QComboBox::currentIndexChanged,
    [this](int idx) {
      this->updateValue(m_raw_value, idx);
    }
  );
}

ScalableSpinBoxValue::~ScalableSpinBoxValue() noexcept {
  if (spinBox != nullptr) {
    delete spinBox;
  }
  if (comboBox != nullptr) {
    delete comboBox;
  }
  if (m_horizontalLayout != nullptr) {
    delete m_horizontalLayout;
  }
}

int ScalableSpinBoxValue::currentRawValue() const noexcept {
  return m_raw_value;
}

double ScalableSpinBoxValue::currentScaledValue() const noexcept {
  return m_scaled_value;
}

QString ScalableSpinBoxValue::currentUnitsText() const noexcept {
  return m_units_text;
}

double ScalableSpinBoxValue::currentScalingRatio() const noexcept {
  return m_scales[m_scaling];
}

int ScalableSpinBoxValue::setRawValue(int x) {
  if (x == m_raw_value) { return m_raw_value; }
  int retval = m_raw_value;
  updateValue(x, m_scaling);
  QSignalBlocker spinBoxBlocker(spinBox);
  spinBox->setValue(x);
  return retval;
}

QString ScalableSpinBoxValue::setUnitsText(const QString& units) {
  if (units == m_units_text) { return m_units_text; }
  QString retstr = m_units_text;
  m_units_text = units;
  for (int i = 0 ; i < m_prefixes.size() ; i ++) {
    comboBox->setItemText(
      i,
      (m_prefixes[i] == ' ') ? (QString("")) : (QString(1, m_prefixes[i])) + m_units_text
    );
  }
  emit unitsTextChanged(m_units_text);
  return retstr;
}

int ScalableSpinBoxValue::setScaleIndex(int index) {
  if (index == m_scaling) { return m_scaling; }
  int retval = m_scaling;
  m_scaling = index;
  updateValue(m_raw_value, m_scaling);
  QSignalBlocker comboBoxBlocker(comboBox);
  comboBox->setCurrentIndex(m_scaling);
  return retval;
}

void ScalableSpinBoxValue::updateValue(int rawValue, int scaleIndex) {
  if (rawValue != m_raw_value) { 
    m_raw_value = rawValue; 
    emit rawValueChanged(m_raw_value);
  }
  if (scaleIndex != m_scaling) { 
    m_scaling = scaleIndex; 
    emit scalingRatioChanged(m_scales[m_scaling]);
  }
  const double new_scaled_value = static_cast<double>(m_raw_value) * m_scales[m_scaling];
  if (m_scaled_value != new_scaled_value) {
    m_scaled_value = new_scaled_value;
    emit scaledValueChanged(m_scaled_value);
  }
}
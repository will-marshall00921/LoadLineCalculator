// LoadLineCalculator -- ScalableSpinBoxValue.cpp
// Author: Will Marshall
// Created: 7/18/2026, 3:07:23 PM

#include "ScalableSpinBoxValue.hpp"
#include <QtCore/QSignalBlocker>
#ifdef DEBUG_BUILD
#include <QtCore/QDebug>
#endif // DEBUG_BUILD

ScalableSpinBoxValue::ScalableSpinBoxValue(QWidget* parent)
  : QWidget { parent }
  , m_horizontalLayout { nullptr }
  , spinBox { nullptr }
  , comboBox { nullptr }
  , m_scaled_value { 0. }
  , m_raw_value { 0. }
  , m_scaling { 0 }
  , m_units_text { "--" }
  {
  m_horizontalLayout = new QHBoxLayout(this);
  m_horizontalLayout->setObjectName("horizontalLayout");
  spinBox = new QDoubleSpinBox(this);
  spinBox->setObjectName("spinBox");
  spinBox->setMinimum(0);
  spinBox->setMaximum(999.99);
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
  m_scales.append(1.E-9);
  m_prefixes.append('p');
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
    &QDoubleSpinBox::valueChanged,
    [this](double x) {
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

double ScalableSpinBoxValue::currentRawValue() const noexcept {
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

double ScalableSpinBoxValue::setRawValue(double x) {
  if (x == m_raw_value) { return m_raw_value; }
  double retval = m_raw_value;
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
      (m_prefixes[i] == ' ') ? (m_units_text) : (QString(1, m_prefixes[i])) + m_units_text
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

void ScalableSpinBoxValue::enablePlaceholder() {
  QSignalBlocker comboBoxBlocker(comboBox);
  QSignalBlocker spinBoxBlocker(spinBox);
  comboBox->setPlaceholderText("--");
  spinBox->setSpecialValueText("--");
}

void ScalableSpinBoxValue::updateValue(double rawValue, int scaleIndex) {
  if (rawValue != m_raw_value) { 
    m_raw_value = rawValue; 
    emit rawValueChanged(m_raw_value);
    #ifdef DEBUG_BUILD
    qDebug() << "[ScalableSpinBoxValue]{updateValue} New raw value: " << m_raw_value;
    #endif // DEBUG_BUILD
  }
  if (scaleIndex != m_scaling) { 
    m_scaling = scaleIndex; 
    emit scalingRatioChanged(m_scales[m_scaling]);
    #ifdef DEBUG_BUILD
    qDebug() << "[ScalableSpinBoxValue]{updateValue} New scale prefix: " << m_prefixes[m_scaling];
    #endif // DEBUG_BUILD
  }
  const double new_scaled_value = (m_raw_value * m_scales[m_scaling]);
  if (m_scaled_value != new_scaled_value) {
    m_scaled_value = new_scaled_value;
    emit scaledValueChanged(m_scaled_value);
    #ifdef DEBUG_BUILD
    qDebug() << "[ScalableSpinBoxValue]{updateValue} New scaled value: " << m_scaled_value;
    #endif // DEBUG_BUILD
  }
}
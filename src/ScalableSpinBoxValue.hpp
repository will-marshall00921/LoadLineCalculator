// LoadLineCalculator -- ScalableSpinBoxValue.hpp
// Author: Will Marshall
// Created: 7/18/2026, 2:58:06 PM

#ifndef SCALABLESPINBOXVALUE_HPP
#define SCALABLESPINBOXVALUE_HPP

#include <QtWidgets/QWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtCore/QString>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QList>

class ScalableSpinBoxValue : public QWidget {
  Q_OBJECT

  double m_scaled_value;
  int m_raw_value;
  int m_scaling;
  QString m_units_text;
  QList<double> m_scales;
  QList<char> m_prefixes;

  QHBoxLayout* m_horizontalLayout;

  public:
    QSpinBox* spinBox;
    QComboBox* comboBox;

    ScalableSpinBoxValue(QWidget* parent = nullptr);

    ~ScalableSpinBoxValue() noexcept;

    int currentRawValue() const noexcept;
    double currentScaledValue() const noexcept;
    QString currentUnitsText() const noexcept;
    double currentScalingRatio() const noexcept;

    Q_SIGNAL void scalingRatioChanged(double scale);
    Q_SIGNAL void rawValueChanged(int rawValue);
    Q_SIGNAL void scaledValueChanged(double scaledValue);
    Q_SIGNAL void unitsTextChanged(QString units);

    Q_SLOT int setRawValue(int x);
    Q_SLOT QString setUnitsText(const QString& units);
    Q_SLOT int setScaleIndex(int index);
    Q_SLOT void enablePlaceholder();

  private:
    void updateValue(int rawValue, int scaleIndex);
};

#endif // SCALABLESPINBOXVALUE_HPP
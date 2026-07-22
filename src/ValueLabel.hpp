// LoadLineCalculator -- ValueLabel.hpp
// Author: Will Marshall
// Created: 7/18/2026, 11:33:40 AM

#ifndef VALUELABEL_HPP
#define VALUELABEL_HPP

#include "BaseValueLabel.hpp"

class ValueLabel : public BaseValueLabel<double> {
  Q_OBJECT

  int m_precision;
  char m_format;

  protected:
    virtual QString formatDisplayString() const noexcept override {
      return (
        QString::number(m_value, m_format, m_precision) + " " + m_units_str
      );
    }

  public:
    ValueLabel(QWidget* parent = nullptr)
      : BaseValueLabel { parent }
      , m_precision { 3 }
      , m_format { 'f' }
      {}
    
    virtual ~ValueLabel() noexcept override {}

    inline int currentPrecision() const noexcept { return m_precision; }

    inline char currentFormat() const noexcept { return m_format; }
    
    Q_SIGNAL void precisionChanged(int precision);

    Q_SIGNAL void formatChanged(char format);

    Q_SLOT int setPrecision(int precision) {
      int retval = currentPrecision();
      m_precision = precision;
      if (retval != precision) {
        this->setText(formatDisplayString());
        if (m_display_placeholder) { m_display_placeholder = false; }
        emit precisionChanged(m_precision);
      }
      return retval;
    }

    Q_SLOT char setFormat(char format) {
      char retval = currentFormat();
      m_format = format;
      if (retval != format) {
        this->setText(formatDisplayString());
        if (m_display_placeholder) { m_display_placeholder = false; }
        emit formatChanged(m_format);
      }
      return retval;
    }
};

#endif // VALUELABEL_HPP
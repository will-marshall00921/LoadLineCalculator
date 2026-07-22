// LoadLineCalculator -- IntValueLabel.hpp
// Author: Will Marshall
// Created: 7/18/2026, 12:15:18 PM

#ifndef INTVALUELABEL_HPP
#define INTVALUELABEL_HPP

#include "BaseValueLabel.hpp"

class IntValueLabel : public BaseValueLabel<int> {
  Q_OBJECT

  int m_base;
  
  protected:
    virtual QString formatDisplayString() const noexcept override {
      return (
        QString::number(m_value, m_base) + " " + m_units_str
      );
    }

  public:
    IntValueLabel(QWidget* parent = nullptr)
      : BaseValueLabel { parent }
      , m_base { 10 }
      {}

    virtual ~IntValueLabel() noexcept override {}

    inline int currentBase() const noexcept { return m_base; }

    Q_SIGNAL void baseChanged(int base);

    Q_SLOT int setBase(int base) {
      char retval = currentBase();
      m_base = base;
      if (retval != base) {
        this->setText(formatDisplayString());
        emit baseChanged(m_base);
      }
      return retval;
    }
};

#endif // INTVALUELABEL_HPP
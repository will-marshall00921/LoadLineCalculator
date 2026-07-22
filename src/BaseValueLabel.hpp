// LoadLineCalculator -- BaseValueLabel.hpp
// Author: Will Marshall
// Created: 7/18/2026, 12:17:30 PM

#ifndef BASEVALUELABEL_HPP
#define BASEVALUELABEL_HPP

#include <QtWidgets/QLabel>
#include <QtCore/QString>
#include <QtCore/QPair>

template <typename _Tp>
class BaseValueLabel : public QLabel {
  protected:
    _Tp m_value;
    QString m_units_str;
    QString m_placeholder_text;
    bool m_display_placeholder;

    virtual QString formatDisplayString() const noexcept {
      return (
        QString::number(m_value) + " " + m_units_str
      );
    }

  public:
    BaseValueLabel(QWidget* parent = nullptr)
      : QLabel { parent }
      , m_value { static_cast<_Tp>(0) }
      , m_placeholder_text { "--" }
      , m_display_placeholder { true }
      {
      this->setText(m_placeholder_text);
    }

    virtual ~BaseValueLabel() noexcept {}

    bool isPlaceholderEnabled() const noexcept {
      return m_display_placeholder;
    }

    QString currentPlaceholderText() const noexcept {
      return m_placeholder_text;
    }

    _Tp currentValue() const noexcept {
      return m_value;
    }

    QString currentUnits() const noexcept {
      return m_units_str;
    }

    QPair<_Tp, QString> currentValueAndUnits() const noexcept {
      return QPair<_Tp, QString>(m_value, m_units_str);
    }

    Q_SIGNAL void valueOrUnitsChanged(_Tp x, QString units) {}

    Q_SLOT _Tp setValue(_Tp x) {
      _Tp retval = currentValue();
      m_value = x;
      if (retval != x) {
        this->setText(formatDisplayString());
        if (m_display_placeholder) { m_display_placeholder = false; }
        emit valueOrUnitsChanged(m_value, m_units_str);
      }
      return retval;
    }

    Q_SLOT QString setUnits(const QString& units) {
      QString retstr = currentUnits();
      m_units_str = units;
      if (retstr != units) {
        this->setText(formatDisplayString());
        if (m_display_placeholder) { m_display_placeholder = false; }
        emit valueOrUnitsChanged(m_value, m_units_str);
      }
      return retstr;
    }

    Q_SLOT QPair<_Tp, QString> setValueAndUnits(_Tp x, const QString& units) {
      QPair<_Tp, QString> retpair = currentValueAndUnits();
      m_value = x;
      m_units_str = units;
      if (
        (retpair.first != x)
          || (retpair.second != units)
      ) {
        this->setText(formatDisplayString());
        if (m_display_placeholder) { m_display_placeholder = false; }
        emit valueOrUnitsChanged(m_value, m_units_str);
      }
      return retpair;
    }

    Q_SLOT bool setPlaceholderEnabled(bool enabled) {
      bool retval = isPlaceholderEnabled();
      m_display_placeholder = enabled;
      if (retval != enabled) {
        this->setText(
          (m_display_placeholder)
            ? (m_placeholder_text)
            : (formatDisplayString())
        );
      }
      return retval;
    }

    Q_SLOT QString setPlaceholderText(const QString& ph) {
      QString retstr = currentPlaceholderText();
      m_placeholder_text = ph;
      if (m_display_placeholder) {
        this->setText(m_placeholder_text);
      }
      return retstr;
    }
};

#endif // BASEVALUELABEL_HPP
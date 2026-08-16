// LoadLineCalculator -- AboutPage.hpp
// Author: Will Marshall
// Created: 8/12/2026, 10:05:47 PM

#ifndef ABOUTPAGE_HPP
#define ABOUTPAGE_HPP

#include <QtWidgets/QDialog>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtCore/QString>

class AboutPage : public QDialog {
  Q_OBJECT

  QVBoxLayout* topLevelLayout;
  QScrollArea* scrollArea;
  QWidget* scrollAreaContentsWidget;
  QVBoxLayout* scrollAreaLayout;
  QLabel* aboutPageContents;

  public:
    explicit AboutPage(QWidget* parent = nullptr);

    ~AboutPage() noexcept;

  private:
    static QString generateContents();
    void setupUi();
};

#endif // ABOUTPAGE_HPP
// LoadLineCalculator -- TubeDataDialog.showCSVHelp.cpp
// Author: Will Marshall
// Created: 8/6/2026, 5:44:30 PM

#include "TubeDataDialog.hpp"
#include <QtWidgets/QLabel>
#include "ui_csvhelp.h"

class CSVHelp : public QDialog {
  Ui::CSVHelp* ui;
  
  public:
    explicit CSVHelp(QWidget* parent = nullptr)
      : QDialog { parent }
      , ui { new Ui::CSVHelp }
      {
      ui->setupUi(this);
    }

    ~CSVHelp() noexcept {
      if (ui != nullptr) {
        delete ui;
      }
    }
};

void TubeDataDialog::showCSVHelp() {
  CSVHelp* helpDialog = new CSVHelp(this);
  helpDialog->setAttribute(Qt::WA_DeleteOnClose);
  helpDialog->show();
}
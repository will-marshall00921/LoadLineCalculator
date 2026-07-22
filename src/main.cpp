// LoadLineCalculator -- main.cpp
// Author: Will Marshall
// Created: 7/18/2026, 5:04:37 PM

#include <QtWidgets/QApplication>
#include "mainwindow.hpp"

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  return a.exec();
}
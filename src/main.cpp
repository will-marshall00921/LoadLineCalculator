// LoadLineCalculator -- main.cpp
// Author: Will Marshall
// Created: 7/18/2026, 5:04:37 PM

#include <QtWidgets/QApplication>
#include "mainwindow.hpp"
#include <QtCore/QStandardPaths>
#ifdef DEBUG_BUILD
#include <QtCore/QDebug>
#endif // DEBUG_BUILD
#include <QtCore/QDir>
#include <QtWidgets/QProgressDialog>
#include <QtCore/QThread>
#include "FirstTimeSetup.hpp"

int check_for_packing(int argc, char* argv[]) {
  if (argc <= 1) {
    return -1;
  }
  int pack_enable = -1;
  for (int argNum = 1 ; argNum < argc ; argNum ++) {
    QString argStr = argv[argNum];
    if ((argStr == "--pack") || (argStr == "-p")) {
      pack_enable = ((argNum + 2) < argc)
        ? (argNum)
        : (-1);
      break;
    }
  }
  return pack_enable;
}

int main(int argc, char* argv[]) {
  // check for special run mode to pack
  const int pack_enable = check_for_packing(argc, argv);
  if (pack_enable != -1) {
    QString inputDirPath(argv[pack_enable+1]);
    QString outputFilePath(argv[pack_enable+2]);
    if (!FirstTimeSetup::packDirectory(inputDirPath, outputFilePath)) {
      return -1;
    }
    return 0;
  }
  QApplication a(argc, argv);
  // Perform first time setup if necessary
  FirstTimeSetup* setup = new FirstTimeSetup;
  if (setup->necessary()) {
    setup->perform();
  }
  delete setup;
  MainWindow w;
  w.show();
  return a.exec();
}
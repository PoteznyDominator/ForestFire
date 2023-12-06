#include "mainwindow.h"
#include "centralwidget.h"

MainWindow::MainWindow(QWidget* parent)
   : QMainWindow(parent) {
   setCentralWidget(new CentralWidget(this));
   setMinimumSize(1080, 720);
}

MainWindow::~MainWindow() {}

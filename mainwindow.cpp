#include "mainwindow.h"

#include "./ui_mainwindow.h"

#include "server.h"

#include <QLineSeries>
#include <utility>

using Server = server::Server;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), numNode(0){
  ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }

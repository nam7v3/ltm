#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "server.h"

#include <QLineSeries>
#include <utility>

using Server = server::Server;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), numNode(0){
  ui->setupUi(this);

  connect(Server::instance(), &Server::nodeAdded, [this](){
      numNode++;
      ui->nodeNumber->setText("Node: " + QString::number(numNode));
  });

  connect(Server::instance(), &Server::nodeRemoved, [this](){
    numNode--;
    ui->nodeNumber->setText("Node: " + QString::number(numNode));
  });
}

MainWindow::~MainWindow() { delete ui; }

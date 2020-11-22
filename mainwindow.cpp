#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
}

//void MainWindow::paintEvent(QPaintEvent *)
//{
//  QPainter painter(this);
//  painter.setPen(QPen(Qt::black, 3, Qt::SolidLine));

//  auto value = std::min(this->width(), this->height());

//  painter.drawEllipse(QRect(10, 10,value, value));

//}

MainWindow::~MainWindow()
{
  delete ui;
}

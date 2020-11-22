#include "mainwindow.h"
#include <QApplication>
#include "customdial.h"
#include <QVBoxLayout>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;

//  QWidget *obj = new QWidget;
  CustomDial *dial = new CustomDial(nullptr);
  CustomDial *sDial = new CustomDial(nullptr);
  //CustomDial *bDial = new CustomDial(nullptr);

  QVBoxLayout *layout = new QVBoxLayout;
  QVBoxLayout *layout2 = new QVBoxLayout;


  QWidget *window = new QWidget;
  window->setGeometry(0,0,400,400);

  QWidget *window2 = new QWidget;
  window2->setGeometry(500,500, 400, 400);

  sDial->setDefaultValues(1000, 6, 1500, 5, 1, -10, 180);

  dial->setDefaultValues(0, -180, -90, 13, 50, 300, 240);
  dial->setStyle(CustomDial::Dracula);

  layout->addWidget(dial);
  layout2->addWidget(sDial);

  window->setLayout(layout);
  window->setStyleSheet("* {background-color: rgb(62, 70, 73)}");
  window->show();

  window2->setLayout(layout2);
  window2->show();
 // bDial->show();

  return a.exec();
}

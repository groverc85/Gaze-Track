#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <unistd.h>
#include "QtGui"
#include "QEvent"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

void MainWindow::changexy(int m, int n)
{
  ui->label->setText(tr("Now the position of your eyes is x:%1,y:%2").arg(m).arg(n));
  ui->label_2->setPixmap(QPixmap("/Users/chenjiongjian/Desktop/SRTP-FINAL/gazetrackfinal/dot.png"));
  ui->label_2->move(m,n);
}


MainWindow::~MainWindow()
{
    delete ui;
}

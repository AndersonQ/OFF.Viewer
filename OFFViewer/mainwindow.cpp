#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "offreader.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    OFFReader *offr = new OFFReader((char *)"/media/Mokona/UFABC/10-Quad/Computacao.Grafica/Proj2/OFF.Viewer/sphere.off");
}

MainWindow::~MainWindow()
{
    delete ui;
}

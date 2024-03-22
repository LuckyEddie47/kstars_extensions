#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_archiver = new archiver("/tmp/test.zip", this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

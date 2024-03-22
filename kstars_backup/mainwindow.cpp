#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_archiver = new archiver("/tmp/test.zip", this);

    connect(ui->goB, &QPushButton::clicked, this, [this] {
        m_archiver->write(QStringList("/home/ed/.local/share/kstars"));
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

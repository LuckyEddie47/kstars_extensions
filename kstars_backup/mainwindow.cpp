#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->addB->setIcon(QIcon::fromTheme("list-add"));
    ui->removeB->setIcon(QIcon::fromTheme("list-remove"));

    m_archiver = new archiver(ui->pathL->text(), this);

    connect(ui->goB, &QPushButton::clicked, this, [this] {
        m_archiver->write(QStringList("/home/ed/.local/share/kstars"));
    });

    connect(ui->browseB, &QPushButton::clicked, this, [this] {
        QFileDialog m_dialog;
        m_dialog.setFileMode(QFileDialog::Directory);
        m_dialog.setOption(QFileDialog::ShowDirsOnly);
        m_dialog.exec();
        ui->pathL->setText(m_dialog.selectedFiles().at(0));
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

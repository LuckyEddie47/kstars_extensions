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

    m_archiver = new archiver(this);
    m_model = new QStringListModel(this);

    connect(ui->goB, &QPushButton::clicked, this, [this] {
        if (mode == MODE_BACKUP) {
            m_archiver->write(QStringList("/home/ed/.local/share/kstars"));
        } else if (mode ==MODE_RESTORE) {
            m_archiver->extract();
        }
    });

    connect(ui->modeB, &QPushButton::clicked, this, [this] {
        if (mode == MODE_BACKUP) {
            mode = MODE_RESTORE;
            ui->modeB->setText("Restore");
        } else if (mode == MODE_RESTORE) {
            mode = MODE_BACKUP;
            ui->modeB->setText("Backup");
        }
        ui->pathL->setText("/home");
    });

    connect(ui->browseB, &QPushButton::clicked, this, [this] {
        QFileDialog m_dialog;
        if (mode == MODE_BACKUP) {
            m_dialog.setFileMode(QFileDialog::Directory);
            m_dialog.setOption(QFileDialog::ShowDirsOnly);
        } else if (mode == MODE_RESTORE) {
            m_dialog.setFileMode(QFileDialog::ExistingFile);
            m_dialog.setNameFilter(QString("*.tar.gz"));
        }
        m_dialog.setDirectory(ui->pathL->text().left(ui->pathL->text().lastIndexOf("/")));
        m_dialog.exec();

        connect(&m_dialog, &QFileDialog::fileSelected, this, [=](const QString &file) {
            ui->pathL->setText(file);
            m_archiver->setArchivePath(file);
            if (mode == MODE_RESTORE) {
                m_model->setStringList(m_archiver->read());
                ui->listView->setModel(m_model);
            }
        });
//        ui->pathL->setText(m_dialog.selectedFiles().at(0));
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

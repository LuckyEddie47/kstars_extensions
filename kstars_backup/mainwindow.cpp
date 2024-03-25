#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    m_archiver = new archiver(this);
    m_model = new QStringListModel(this);

    ui->setupUi(this);
    ui->addB->setIcon(QIcon::fromTheme("list-add"));
    ui->removeB->setIcon(QIcon::fromTheme("list-remove"));
    ui->listView->setModel(m_model);
    setNewPath("/tmp");

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
            ui->modeB->setText(tr("Restore"));
            ui->goB->setText(tr("Restore from Backup"));
            ui->addB->setEnabled(false);
            ui->removeB->setEnabled(false);
        } else if (mode == MODE_RESTORE) {
            mode = MODE_BACKUP;
            ui->modeB->setText(tr("Backup"));
            ui->goB->setText(tr("Create Backup"));
            ui->addB->setEnabled(true);
            ui->removeB->setEnabled(true);
        }
        setNewPath("/tmp");
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

        QString m_dir = ui->pathL->text();
        if (m_dir.count("/") > 1 && m_dir.endsWith(".tar.gz")  && mode == MODE_RESTORE) {
            m_dir = m_dir.left((m_dir.lastIndexOf("/")));
        }
        m_dialog.setDirectory(m_dir);
        if (m_dialog.exec()) {
            setNewPath(m_dialog.selectedFiles().at(0));
        }
    });

    connect(m_archiver, &archiver::readSets, this, [this] (QStringList sets) {
        m_model->setStringList(sets);
    });
}

void MainWindow::setNewPath(const QString &path)
{
    ui->pathL->setText(path);
    m_archiver->setArchivePath(path);
    if (mode == MODE_RESTORE && path.endsWith(".tar.gz")) {
        m_archiver->read();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

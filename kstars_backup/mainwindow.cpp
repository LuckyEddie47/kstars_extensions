#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    m_archiver = new archiver(this);
    m_model = new QStringListModel(this);
    chosenPaths = new QStringList;

    ui->setupUi(this);
    ui->addB->setIcon(QIcon::fromTheme("list-add"));
    ui->removeB->setIcon(QIcon::fromTheme("list-remove"));
    ui->listView->setModel(m_model);
    ui->goB->setEnabled(false)    ;
    setNewPath("/tmp");

    connect(ui->modeB, &QPushButton::clicked, this, [this] {
        if (mode == MODE_BACKUP) {
            mode = MODE_RESTORE;
            ui->modeB->setText(tr("Restore"));
            ui->addB->setEnabled(false);
            ui->removeB->setEnabled(false);
            ui->goB->setText(tr("Restore from Bakcup"));
        } else if (mode == MODE_RESTORE) {
            mode = MODE_BACKUP;
            ui->modeB->setText(tr("Backup"));
            ui->addB->setEnabled(true);
            ui->removeB->setEnabled(true);
            ui->goB->setText(tr("Create Backup"));
        }
        m_model->setStringList(QStringList());
        if (m_model->rowCount() > 0) {
            ui->goB->setEnabled(true);
        } else {
            ui->goB->setEnabled(false);
        }
        setNewPath("/tmp");
    });

    connect(ui->addB, &QPushButton::clicked, this, [this] {
        QFileDialog m_addDialog;
        m_addDialog.setDirectory(lastDir);
        m_addDialog.setFileMode(QFileDialog::Directory);
        m_addDialog.setOption(QFileDialog::ShowDirsOnly);
        if (m_addDialog.exec()) {
            chosenPaths->append(m_addDialog.selectedFiles());
            m_model->setStringList(*chosenPaths);
        }
        if (m_model->rowCount() > 0) {
            ui->goB->setEnabled(true);
        } else {
            ui->goB->setEnabled(false);
        }
    });

    connect(ui->removeB, &QPushButton::clicked, this, [this] {
        chosenPaths->removeLast();
        m_model->setStringList(*chosenPaths);
        if (m_model->rowCount() > 0) {
            ui->goB->setEnabled(true);
        } else {
            ui->goB->setEnabled(false);
        }
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
        lastDir = m_dir;
        if (m_dialog.exec()) {
            setNewPath(m_dialog.selectedFiles().at(0));
            ui->modeB->setEnabled(false);
            ui->browseB->setEnabled(false);
            ui->goB->setEnabled(false);
        }
    });

    connect(m_archiver, &archiver::readSets, this, [this] (QStringList sets) {
        m_model->setStringList(sets);
        ui->modeB->setEnabled(true);
        ui->browseB->setEnabled(true);
        if (m_model->rowCount() > 0) {
            ui->goB->setEnabled(true);
        } else {
            ui->goB->setEnabled(false);
        }
    });

    connect(ui->goB, &QPushButton::clicked, this, [this] {
        if (mode == MODE_BACKUP) {
            m_archiver->write(QStringList("/home/ed/.local/share/kstars"));
        } else if (mode ==MODE_RESTORE) {
            m_archiver->extract();
        }
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

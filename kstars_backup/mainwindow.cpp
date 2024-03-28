#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QLocale>

MainWindow::MainWindow(const QString &appFilePath, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    m_confChecker = new confChecker(appFilePath, this);
    m_archiver = new archiver(this);
    m_model = new QStringListModel(this);
    chosenPaths = new QStringList;

    // Setup UI
    ui->setupUi(this);
    ui->addB->setIcon(QIcon::fromTheme("list-add"));
    ui->removeB->setIcon(QIcon::fromTheme("list-remove"));
    ui->listView->setModel(m_model);
    ui->goB->setEnabled(false);
    ui->progressBar->setVisible(false);
    ui->statusL->setText(tr("Idle"));
    setNewPath("/tmp");

    connect(ui->modeB, &QPushButton::clicked, this, [this] {
        if (mode == MODE_BACKUP) {
            mode = MODE_RESTORE;
            ui->modeB->setText(tr("Restore"));
            ui->addB->setEnabled(false);
            ui->removeB->setEnabled(false);
            ui->goB->setText(tr("Restore from Backup"));
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
        ui->statusL->setText(tr("Idle"));
        ui->usedL->setText("");
        ui->freeL->setText("");
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
            m_archiver->getSourceSize(m_model->stringList());
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
            ui->statusL->setText(tr("Getting size"));
        } else {
            ui->goB->setEnabled(false);
        }
    });

    connect(m_archiver, &archiver::archiveSize, this, [this] (ulong size) {
        QLocale m_locale = this->locale();
        QString displaySize = m_locale.formattedDataSize(size);
        ui->usedL->setText(tr("Archive: %1").arg(displaySize));
        m_archiver->getDestinationSpace(m_model->stringList().at(0));
    });

    connect(m_archiver, &archiver::destinationSpace, this, [this] (ulong space) {
        QLocale m_locale = this->locale();
        QString displaySpace = m_locale.formattedDataSize(space);
        ui->freeL->setText(tr("Space: %1").arg(displaySpace));
        ui->statusL->setText(tr("Idle"));
    });

    connect(m_archiver, &archiver::sourceSize, this, [this] (ulong used) {
        QLocale m_locale = this->locale();
        QString displayUsed = m_locale.formattedDataSize(used);
        ui->usedL->setText(tr("Source(s): %1").arg(displayUsed));
        ui->statusL->setText(tr("Idle"));
    });

    connect(ui->goB, &QPushButton::clicked, this, [this] {
        if (mode == MODE_BACKUP) {
            m_archiver->write(m_model->stringList());
        } else if (mode ==MODE_RESTORE) {
            m_archiver->extract();
        }
    });

    connect(m_confChecker, &confChecker::pathsFound, this, [this] (const QStringList &paths) {
        m_model->setStringList(paths);
    });

}

void MainWindow::setNewPath(const QString &path)
{
    ui->pathL->setText(path);
    m_archiver->setArchivePath(path);
    if (mode == MODE_RESTORE && path.endsWith(".tar.gz")) {
        ui->statusL->setText(tr("Reading archive"));
        m_archiver->read();
    } else if (mode == MODE_BACKUP) {
        m_archiver->getDestinationSpace(path);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

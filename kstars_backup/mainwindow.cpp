#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QLocale>
#include <QMessageBox>
#include <QTimer>

MainWindow::MainWindow(const QString &appFilePath, const QString &ks_version, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    m_confChecker = new confChecker(appFilePath, this);
    m_archiver = new archiver(ks_version, this);
    m_kstarsinterface = new kstarsinterface(this);
    m_model = new QStringListModel(this);
    chosenPaths = new QStringList;

    // Setup UI
    ui->setupUi(this);
    ui->addB->setIcon(QIcon::fromTheme("list-add"));
    ui->removeB->setIcon(QIcon::fromTheme("list-remove"));
    ui->listView->setModel(m_model);
    ui->goB->setEnabled(false);
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
            m_archiver->getSourceSize(m_model->stringList());
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
            addLog(tr("Getting size"));
        } else {
            ui->goB->setEnabled(false);
        }
    });

    connect(m_archiver, &archiver::archiveSize, this, [this] (ulong size) {
        archiveSize = size;
        QLocale m_locale = this->locale();
        QString displaySize = m_locale.formattedDataSize(size);
        addLog(tr("Archive: %1").arg(displaySize), USEDL);
        m_archiver->getDestinationSpace(m_model->stringList().at(0));
    });

    connect(m_archiver, &archiver::destinationSpace, this, [this] (ulong space) {
        spaceAvailable = space;
        QLocale m_locale = this->locale();
        QString displaySpace = m_locale.formattedDataSize(space);
        ui->freeL->setVisible(true);
        addLog(tr("Space: %1").arg(displaySpace), FREEL);
        ui->statusL->setText(tr("Idle"));
        if (archiveSize > spaceAvailable) {
            ui->goB->setEnabled(false);
            addLog(tr("Insufficient space"));
        }
    });

    connect(m_archiver, &archiver::sourceSize, this, [this] (ulong used) {
        QLocale m_locale = this->locale();
        QString displayUsed = m_locale.formattedDataSize(used);
        addLog(tr("Source(s): %1").arg(displayUsed), USEDL);
        addLog(tr("Idle"));
    });

    connect(m_archiver, &archiver::done, this, [this] {
        addLog(tr("Complete"));
        ui->goB->setEnabled(true);
    });

    connect(m_archiver, &archiver::writing, this, [this] (const QString archiveName) {
        addLog(tr("Writing: %1").arg(archiveName), NONEL);
    });

    connect(ui->goB, &QPushButton::clicked, this, [this] {
        ui->goB->setEnabled(false);
        if (mode == MODE_BACKUP) {
            addLog(tr("Archiving..."));
            m_archiver->write(m_model->stringList());
        } else if (mode == MODE_RESTORE) {
            if (archiveSize > spaceAvailable) {
                QMessageBox m_msgbox1(QMessageBox::Question,
                                      tr("No room for backup"),
                                      tr("It is recommended to create a new backup before restoring, "
                                      "however there is insufficient space available.\n"
                                      "Continue without a new backup?"),
                                      QMessageBox::Yes | QMessageBox::No,
                                      this);
                m_msgbox1.setDefaultButton(QMessageBox::No);
                if (m_msgbox1.exec() == QMessageBox::Yes) {
                    addLog(tr("Restoring."));
                    m_archiver->extract();
                }
            } else {
                QMessageBox m_msgbox2(QMessageBox::Question,
                                      tr("Backup before restoring?"),
                                      tr("It is recommended to create a new backup before restoring.\n"
                                     "Create a new backup?"),
                                      QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                      this);
                m_msgbox2.setDefaultButton(QMessageBox::Yes);
                int ret = m_msgbox2.exec();
                if (ret == QMessageBox::Yes) {
                    addLog(tr("Archiving."));
                    m_archiver->write(m_model->stringList(), true);
                    connect(m_archiver, &archiver::done, this, [this] {
                        addLog(tr("Restoring."));
                        m_archiver->extract();
                        }, Qt::SingleShotConnection);
                } else if (ret == QMessageBox::No) {
                    addLog(tr("Restoring."));
                    m_archiver->extract();
                }
            }
        }
    });

    connect(m_confChecker, &confChecker::pathsFound, this, [this] (const QStringList &paths) {
        *chosenPaths = paths;
        m_model->setStringList(*chosenPaths);
        if (m_model->rowCount() > 0) {
            ui->goB->setEnabled(true);
            m_archiver->getSourceSize(m_model->stringList());
        } else {
            ui->goB->setEnabled(false);
        }
    });

    connect(m_confChecker, &confChecker::errorMessage, this, &MainWindow::handleError);
    connect(m_kstarsinterface, &kstarsinterface::errorMessage, this, &MainWindow::handleError);

    QTimer *m_timer =new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, [this] {
        if (ui->statusL->text().endsWith("...")) {
            ui->statusL->setText(ui->statusL->text().left(ui->statusL->text().length() - 2));
        } else if (ui->statusL->text().endsWith("..")) {
            ui->statusL->setText(ui->statusL->text().append("."));
        } else if (ui->statusL->text().endsWith(".")) {
            ui->statusL->setText(ui->statusL->text().append("."));
        }
    });
    m_timer->start(2000);
}

void MainWindow::begin()
{
    m_kstarsinterface->checkDBus();
    m_confChecker->start();
}

void MainWindow::setNewPath(const QString &path)
{
    ui->pathL->setText(path);
    m_archiver->setArchivePath(path);
    if (mode == MODE_RESTORE && path.endsWith(".tar.gz")) {
        addLog(tr("Reading archive"));
        m_archiver->read();
    } else if (mode == MODE_BACKUP) {
        m_archiver->getDestinationSpace(path);
    }
}

void MainWindow::handleError(const QString &errorDetail)
{
    ui->logPTE->appendPlainText(QString("Error: ").append(errorDetail));
}

void MainWindow::addLog(const QString &logMessage, additionalOutput addOut)
{
    ui->logPTE->appendPlainText(logMessage);
    if (addOut == STATUSL) {
        ui->statusL->setText(logMessage);
    } else if (addOut == USEDL) {
        ui->usedL->setText(logMessage);
    } else if (addOut == FREEL) {
        ui->freeL->setText(logMessage);
    }
}

void MainWindow::halt()
{
    m_kstarsinterface->restartKStars();
}

MainWindow::~MainWindow()
{
    delete ui;
}

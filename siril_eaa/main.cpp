#include <QCoreApplication>
#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QtDBus>
#include <QTimer>

#include "log.h"
#include "kstarsinterface.h"
#include "process.h"
#include "bombout.h"

// This is the earlies versin of KStars that this plugin targets
#define MIN_KSTARS_VERSION "3.7.0"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("Siril_EAA");
    QObject *parent;

    logger m_log;
    kstarsinterface m_kstarsinterface;
    process m_process;
    bool okayToProceed = false;

    // Check that the config file exists, is accessible,
    // Check that the config file contains a line starting
    // minimum_kstars_version=xx.yy.zz
    // and that the xx.yy.zz matches the MIN_KSTARS_VERSION
    QString Sirilpath = "";
    QFile Siril;
    QString confFileName = QString("%1%2%3%4")
                               .arg(app.applicationDirPath(), "/", app.applicationName(), ".conf");
    QFile confFile(confFileName);
    if (confFile.exists()) {
        if (confFile.open(QIODevice::ReadOnly) && confFile.isReadable()) {
            QTextStream confTS = QTextStream(&confFile);
            while (!confTS.atEnd()) {
                QString confLine = confTS.readLine();
                if (confLine.contains("minimum_kstars_version=")) {
                    QString minVersion = confLine.right(confLine.length() - (confLine.indexOf("=")) - 1);
                    QStringList minVersionElements = minVersion.split(".");
                    if (minVersionElements.count() == 3) {
                        QList <int> minVersionElementInts;
                        foreach (QString element, minVersionElements) {
                            if (element.toInt() || element == "0") {
                                minVersionElementInts.append(element.toInt());
                            } else break;
                        }
                        if (minVersionElementInts.count() == minVersionElements.count()) {
                            QStringList KStarsVersionElements = QString(MIN_KSTARS_VERSION).split(".");
                            QList <int> KStarsVersionElementInts;
                            foreach (QString element, KStarsVersionElements) {
                                if (element.toInt() || element == "0") {
                                    KStarsVersionElementInts.append(element.toInt());
                                }
                            }
                            if ((minVersionElementInts.at(0) == KStarsVersionElementInts.at(0)) &&
                                (minVersionElementInts.at(1) == KStarsVersionElementInts.at(1)) &&
                                (minVersionElementInts.at(2) == KStarsVersionElementInts.at(2))){
                                okayToProceed = true;
                            }
                        }
                    }
                }
            }

            // Check that the .conf file contains a valid path for Siril
            if (okayToProceed) {
                confTS.seek(0);
                okayToProceed = false;
                while (!confTS.atEnd()) {
                    QString confLine = confTS.readLine();
                    if (confLine.contains("siril_path=")) {
                        Sirilpath = confLine.right(confLine.length() - (confLine.indexOf("=")) - 1);
                        Siril.setFileName(Sirilpath);
                        if (Siril.exists()) {
                            okayToProceed = true;
                        } else {
                            qDebug() << (QString("Conf file %1 contains path '%2' which does not exist")
                                             .arg(confFileName, Sirilpath));
                            bombout();
                        }
                    }
                }
            } else qDebug() << QString(".conf file %1 does not contain a valid minimum_kstars_version string").append(confFileName);
        } else qDebug() << QString("Can't access .conf file %1").arg(confFileName);
    } else qDebug() << QString(".conf file %1 disappeared").arg(confFileName);
    if (okayToProceed) qDebug() << QString("Configuration file is okay");
    else bombout();

    // Check that the DBus service is running
    if (okayToProceed) {
        if (m_kstarsinterface.checkDBus()) {
            m_log.out("Connected to DBus session interface");
        } else {
            m_log.out("Can not connect to DBus session interface, is the DBus deamon running?");
            okayToProceed = false;
            bombout();
        }
    }

    // Check that KStars is accessible on the DBus
    if (okayToProceed) {
        if (m_kstarsinterface.checkKStarsService()) {
            m_log.out("Connected to KStars DBus interface");
        } else {
            m_log.out("Can not connect to KStars DBus interface, is KStars running?");
            okayToProceed = false;
            bombout();
        }
    }


    // Check that the Ekos Scheduler is inactive
    if (okayToProceed) {
        switch (m_kstarsinterface.checkSchedulerStatus())
        {
        case SCHEDULER_UNKNOWN:
            m_log.out("Could not determine the state of the Scheduler");
            okayToProceed = false;
            bombout();
            break;
        case SCHEDULER_IDLE:
        case SCHEDULER_PAUSED:
        case SCHEDULER_ABORTED:
            m_log.out("Scheduler is inactive");
            break;
        default:
            m_log.out("Scheduler is in use");
            okayToProceed = false;
            bombout();
        };
    }

    // Check that the Ekos Capture module is inactive
    if (okayToProceed) {
        switch (m_kstarsinterface.checkCaptureStatus())
        {
        case CAPTURE_UNKNOWN:
            m_log.out("Could not determine the state of the Capture module");
            okayToProceed = false;
            bombout();
            break;
        case CAPTURE_IDLE:
        case CAPTURE_COMPLETE:
        case CAPTURE_ABORTED:
            m_log.out("Capture module is inactive");
            break;
        default:
            m_log.out("Capture module is in use");
            okayToProceed = false;
            bombout();
        };
    }

    QObject::connect(&m_kstarsinterface, &kstarsinterface::stopSession, &m_process, &process::stopProgram);

    // Setup Siril
    if (okayToProceed) {
        QObject::connect(&m_process, &process::programStarted, [&m_log] () {
            m_log.out("Siril started");
        });
        QObject::connect(&m_process, &process::programFinished, [&m_kstarsinterface, &m_log, &app] () {
            m_log.out("Siril closed");
            m_log.out("All done");
            bombout();
        });
        QObject::connect(&m_process, &process::sirilMessage, &m_log, &logger::out);
        m_log.out("Starting Siril");
        m_process.startProgram(Sirilpath);
    }

    QTimer::singleShot(2000, &app, [&m_process] () {
        m_process.sendSirilCommand("ping");
    });

    return app.exec();
}

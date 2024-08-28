#include <QCoreApplication>
#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QtDBus>

#include "log.h"
#include "kstarsinterface.h"
#include "process.h"
#include "bombout.h"

// This is the earliest version of KStars that this plugin targets
#define MIN_KSTARS_VERSION "3.7.3"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QTranslator m_translator;
    if (m_translator.load(QLocale(), ":/i18n/launcher")) {
        app.installTranslator(&m_translator);
    }
    if (argc > 0) {
        QString appName = QString::fromLocal8Bit(argv[0]);
        appName = appName.right(appName.length() - (appName.lastIndexOf("/") + 1));
        app.setApplicationName(appName);
    }
    QObject *parent;

    logger m_log;
    kstarsinterface m_kstarsinterface;
    process m_process;

    bool okayToProceed = false;

    // Check that the config file exists, is accessible,
    // Check that the config file contains a line starting
    // minimum_kstars_version=xx.yy.zz
    // and that the xx.yy.zz matches or exceeds the MIN_KSTARS_VERSION
    QString progPath = "";
    QFile prog;
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
                            if (KStarsVersionElementInts.at(0) > minVersionElementInts.at(0)) {
                                okayToProceed = true;
                            } else if ((KStarsVersionElementInts.at(0) == minVersionElementInts.at(0)) &&
                                       (KStarsVersionElementInts.at(1) > minVersionElementInts.at(1))) {
                                okayToProceed = true;
                            } else if ((KStarsVersionElementInts.at(0) == minVersionElementInts.at(0)) &&
                                       (KStarsVersionElementInts.at(1) == minVersionElementInts.at(1)) &&
                                       (KStarsVersionElementInts.at(2) >= minVersionElementInts.at(2))) {
                                okayToProceed = true;
                            }
                        }
                    }
                }
            }

            // Check that the .conf file contains a valid path for an executable
            if (okayToProceed) {
                confTS.seek(0);
                okayToProceed = false;
                while (!confTS.atEnd()) {
                    QString confLine = confTS.readLine();
                    if (confLine.contains("program_path=")) {
                        progPath = confLine.right(confLine.length() - (confLine.indexOf("=")) - 1);
                        prog.setFileName(progPath);
                        if (prog.exists()) {
                            okayToProceed = true;
                        } else {
                            m_log.out(QObject::tr("Conf file %1 contains path '%2' which does not exist")
                                             .arg(confFileName, progPath));
                            bombout();
                        }
                    }
                }
            } else m_log.out(QObject::tr(".conf file %1 does not contain a valid minimum_kstars_version string").arg(confFileName));
        } else m_log.out(QObject::tr("Can't access .conf file %1").arg(confFileName));
    } else m_log.out(QObject::tr(".conf file %1 disappeared").arg(confFileName));
    if (okayToProceed) m_log.out(QObject::tr("Configuration file is okay"));
    else bombout();


    // Check that the DBus service is running
    if (okayToProceed) {
        if (m_kstarsinterface.checkDBus()) {
            m_log.out(QObject::tr("Connected to DBus session interface"));
        } else {
            m_log.out(QObject::tr("Can not connect to DBus session interface, is the DBus deamon running?"));
            okayToProceed = false;
            bombout();
        }
    }

    // Check that KStars is accessible on the DBus
    if (okayToProceed) {
        if (m_kstarsinterface.checkKStarsService()) {
            m_log.out(QObject::tr("Connected to KStars DBus interface"));
        } else {
            m_log.out(QObject::tr("Can not connect to KStars DBus interface, is KStars running?"));
            okayToProceed = false;
            bombout();
        }
    }

    // Check that the Ekos Scheduler is inactive
    if (okayToProceed) {
        switch (m_kstarsinterface.checkSchedulerStatus())
        {
        case SCHEDULER_UNKNOWN:
            m_log.out(QObject::tr("Could not determine the state of the Scheduler"));
            okayToProceed = false;
            bombout();
            break;
        case SCHEDULER_IDLE:
        case SCHEDULER_PAUSED:
        case SCHEDULER_ABORTED:
            m_log.out(QObject::tr("Scheduler is inactive"));
            break;
        default:
            m_log.out(QObject::tr("Scheduler is in use"));
            okayToProceed = false;
            bombout();
        };
    }

   // Check that the Ekos Capture module is inactive
   if (okayToProceed) {
       switch (m_kstarsinterface.checkCaptureStatus())
       {
       case CAPTURE_UNKNOWN:
           m_log.out(QObject::tr("Could not determine the state of the Capture module"));
           okayToProceed = false;
           bombout();
           break;
       case CAPTURE_IDLE:
       case CAPTURE_COMPLETE:
       case CAPTURE_ABORTED:
           m_log.out(QObject::tr("Capture module is inactive"));
           break;
       default:
           m_log.out(QObject::tr("Capture module is in use"));
           okayToProceed = false;
           bombout();
       };
   }

   // Connect stop handlers
   QObject::connect(&m_kstarsinterface, &kstarsinterface::exitRequested, &app, [&] {
       m_process.stopProgram();
   });
   QObject::connect(&m_process, &process::programFinished, &app, [&] {
       QMetaObject::invokeMethod(&app, "quit", Qt::QueuedConnection);
   });

   // Setup program, and connect to status
   if (okayToProceed) {
       QObject::connect(&m_process, &process::programStarted, &app, [&m_log] () {
           m_log.out(QObject::tr("Program started"));
       });
       m_process.startProgram(progPath);
   }

   return app.exec();
}

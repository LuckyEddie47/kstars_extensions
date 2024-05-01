#include <QCoreApplication>
#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QtDBus>

#include "log.h"
#include "kstarsinterface.h"
#include "process.h"
#include "bombout.h"

// This is the earlies version of KStars that this plugin targets
#define MIN_KSTARS_VERSION "3.7.1"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QTranslator m_translator;
    if (m_translator.load(QLocale(), ":/i18n/FC_launcher")) {
        app.installTranslator(&m_translator);
    }
    app.setApplicationName("FC_launcher");
    QObject *parent;

    logger m_log;
    kstarsinterface m_kstarsinterface;
    process m_process;

    bool okayToProceed = false;

    // Check that the config file exists, is accessible,
    // Check that the config file contains a line starting
    // minimum_kstars_version=xx.yy.zz
    // and that the xx.yy.zz matches the MIN_KSTARS_VERSION
    QString FCpath = "";
    QFile FC;
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

            // Check that the .conf file contains a valid path for FireCapture
            if (okayToProceed) {
                confTS.seek(0);
                okayToProceed = false;
                while (!confTS.atEnd()) {
                    QString confLine = confTS.readLine();
                    if (confLine.contains("firecapture_path=")) {
                        FCpath = confLine.right(confLine.length() - (confLine.indexOf("=")) - 1);
                        FC.setFileName(FCpath);
                        if (FC.exists()) {
                            okayToProceed = true;
                        } else {
                            m_log.out(QObject::tr("Conf file %1 contains path '%2' which does not exi st")
                                             .arg(confFileName, FCpath));
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

   QObject::connect(&m_kstarsinterface, &kstarsinterface::exitRequested, &m_process, &process::stopProgram);

   // Setup FireCapture, connect to status and when closed
   // reconnect the camera INDI driver
   if (okayToProceed) {

       QObject::connect(&m_process, &process::programStarted, &app, [&m_log] () {
           m_log.out(QObject::tr("FireCapture started"));
       });

       QObject::connect(&m_process, &process::programFinished, &app, [&m_kstarsinterface, &m_log, &app] () {
           m_log.out(QObject::tr("Reconnecting camera"));
           m_kstarsinterface.reconnectCamera();
           m_log.out(QObject::tr("All done"));
           bombout();
       });

       if (m_process.startScriptUpToDate(FCpath)) {
           m_log.out(QObject::tr("Starting FireCapture"));
           m_process.startProgram(FCpath);
       } else {
           m_log.out(QObject::tr("FireCapture start script does not support --no-confirm and can not be patched"));
           bombout();
       }
   }

   return app.exec();
}

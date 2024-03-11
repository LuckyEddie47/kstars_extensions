#include <QCoreApplication>
#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QtDBus>

#include "log.h"
#include "kstarsinterface.h"
#include "process.h"
#include "bombout.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("FC_launcher");
    QObject *parent;

    logger m_log;
    kstarsinterface m_kstarsinterface;
    process m_process;
    bool okayToProceed = true;

    // Check that the config file exists, is accessible,
    // and contains a path that also exists
    QString FCpath = "";
    QFile FC;
    QString confFileName = QString("%1%2%3%4")
        .arg(app.applicationDirPath(), "/", app.applicationName(), ".conf");
    QFile confFile(confFileName);
    if (confFile.exists()) {
        if (confFile.open(QIODevice::ReadOnly)) {
            QTextStream in (&confFile);
            FCpath = in.readLine();
            FC.setFileName(FCpath);
            if (!FC.exists()) {
                m_log.out(QString("Conf file %1 contains path '%2' which does not exist")
                              .arg(confFileName, FCpath));
                okayToProceed = false;
                bombout();
            }
        } else {
            m_log.out(QString("Conf file %1 exists but could not be opened").arg(confFileName));
            okayToProceed = false;
            bombout();
        }
    } else {
        m_log.out(QString("Conf file %1 does not exist").arg(confFileName));
        okayToProceed = false;
        bombout();
    }

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

    // Setup FireCapture, connect to status and when closed
    // reconnect the camera INDI driver
    if (okayToProceed) {

        QObject::connect(&m_process, &process::programStarted, [&m_log] () {
            m_log.out("FireCapture started");
        });

        QObject::connect(&m_process, &process::programFinished, [&m_kstarsinterface, &m_log, &app] () {
            m_log.out("FireCapture closed, reconnecting camera");
            m_kstarsinterface.reconnectCamera();
            m_log.out("All done");
            bombout();
        });

        m_log.out("Starting FireCapture");
        m_process.startProgram(FCpath);
    }

    return app.exec();
}

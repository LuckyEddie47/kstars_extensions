#include <QCoreApplication>
#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QtDBus>
#include <QTimer>

#include "statemachine.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("Siril_EAA");
    QObject *parent;

    statemachine m_statemachine(QString("%1%2%3").arg(app.applicationDirPath(), "/", app.applicationName()));

//    // Check that the DBus service is running
//    if (okayToProceed) {
//        if (m_kstarsinterface.checkDBus()) {
//            m_log.out("Connected to DBus session interface");
//        } else {
//            m_log.out("Can not connect to DBus session interface, is the DBus deamon running?");
//            okayToProceed = false;
//            bombout();
//        }
//    }
//
//    // Check that KStars is accessible on the DBus
//    if (okayToProceed) {
//        if (m_kstarsinterface.checkKStarsService()) {
//            m_log.out("Connected to KStars DBus interface");
//        } else {
//            m_log.out("Can not connect to KStars DBus interface, is KStars running?");
//            okayToProceed = false;
//            bombout();
//        }
//    }
//
//
//    // Check that the Ekos Scheduler is inactive
//    if (okayToProceed) {
//        switch (m_kstarsinterface.checkSchedulerStatus())
//        {
//        case SCHEDULER_UNKNOWN:
//            m_log.out("Could not determine the state of the Scheduler");
//            okayToProceed = false;
//            bombout();
//            break;
//        case SCHEDULER_IDLE:
//        case SCHEDULER_PAUSED:
//        case SCHEDULER_ABORTED:
//            m_log.out("Scheduler is inactive");
//            break;
//        default:
//            m_log.out("Scheduler is in use");
//            okayToProceed = false;
//            bombout();
//        };
//    }
//
//    // Check that the Ekos Capture module is inactive
//    if (okayToProceed) {
//        switch (m_kstarsinterface.checkCaptureStatus())
//        {
//        case CAPTURE_UNKNOWN:
//            m_log.out("Could not determine the state of the Capture module");
//            okayToProceed = false;
//            bombout();
//            break;
//        case CAPTURE_IDLE:
//        case CAPTURE_COMPLETE:
//        case CAPTURE_ABORTED:
//            m_log.out("Capture module is inactive");
//            break;
//        default:
//            m_log.out("Capture module is in use");
//            okayToProceed = false;
//            bombout();
//        };
//    }
//
//    QObject::connect(&m_kstarsinterface, &kstarsinterface::stopSession, &m_process, &process::stopProgram);

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

    QTimer::singleShot(4000, &app, [&m_process] () {
        m_process.sendSirilCommand("exit");
    });

    return app.exec();
}

/* This is a minimal example of a KStars_extension
 * It demonstrates:
 * Checking for a valid configuration file
 * Checking for a DBus connection to KStars
 * Passing status messages/errors to KStars
 * Monitoring a stop request from KStars and cleanly exiting
 *
 * Note that calls to QCoreApplication::quit() have to be queued here
 * as the Qt Signal/Slot system does not run until the .exec() is called
 */

#include <QCoreApplication>

#include "log.h"
#include "kstarsinterface.h"
#include "process.h"
#include "confchecker.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("example");
    QObject *parent;

    // Create instances
    logger m_log;
    confChecker m_confchecker(QString("%1%2%3").arg(app.applicationDirPath(), "/", app.applicationName()));
    kstarsinterface m_kstarsinterface;
    process m_process;

    // Connect messages
    QObject::connect(&m_confchecker, &confChecker::errorMessage, &m_log, &logger::out);
    QObject::connect(&m_kstarsinterface, &kstarsinterface::errorMessage, &m_log, &logger::out);
    QObject::connect(&m_process, &process::message, &m_log, &logger::out);

    // Connect stop handler
    QObject::connect(&m_kstarsinterface, &kstarsinterface::exitRequested, &app, [&] {
        m_process.stopProcess();
        QMetaObject::invokeMethod(&app, "quit", Qt::QueuedConnection);
    });

    // Start process
    if (m_confchecker.isValid()) {
        if (m_kstarsinterface.kstarsStateIsValid()) {
            m_process.startProcess();
        } else {
            QMetaObject::invokeMethod(&app, "quit", Qt::QueuedConnection);
        }
    } else {
        QMetaObject::invokeMethod(&app, "quit", Qt::QueuedConnection);
    }

    return app.exec();
}

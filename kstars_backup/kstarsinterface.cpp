#include "kstarsinterface.h"
#include "kstarsDBusConf.h"
#include "ekosStatus.h"

#include <QtDBus>
#include <QProcess>
#include <QTimer>

kstarsinterface::kstarsinterface(QObject *parent)
{}

// Ephemeral test for usable DBus
void kstarsinterface::checkDBus()
{
    if (QDBusConnection::sessionBus().isConnected()) {
        checkKStars();
    } else {
        emit errorMessage(tr("Can not connect to DBus session interface, is the DBus deamon running?"));
    }
}

// Ephemeral test for running KStars
void kstarsinterface::checkKStars()
{
    QDBusInterface interface(serviceName, "/", ksInterface, bus, this);
    if (interface.isValid()) {
        checkScheduler();
    } else {
        emit errorMessage(tr("Can not connect to KStars DBus interface, is KStars running?"));
    }
}

// Ephemeral test for idle Ekos Scheduler
void kstarsinterface::checkScheduler()
{
    SchedulerState m_state = SCHEDULER_UNKNOWN;
    QDBusInterface interface(serviceName, pathScheduler);
    if (interface.isValid()) {
        m_state = static_cast<SchedulerState>(interface.property("status").toInt());
        switch (m_state) {
        case SCHEDULER_UNKNOWN:
            emit errorMessage(tr("Could not determine the state of the Scheduler"));
            break;
        case SCHEDULER_IDLE:
        case SCHEDULER_PAUSED:
        case SCHEDULER_ABORTED:
            checkCapture();
            break;
        default:
            emit errorMessage(tr("Scheduler is in use"));
        };
    } else {
        emit errorMessage(tr("Could not determine the state of the Scheduler"));
    }
}

// Ephemeral test for idle Ekos Capture Module
void kstarsinterface::checkCapture()
{
    CaptureState m_state = CAPTURE_UNKNOWN;
    QDBusInterface interface(serviceName, pathCapture);
    if (interface.isValid()) {
        m_state = static_cast<CaptureState>(interface.property("status").toInt());
        switch (m_state)
        {
        case CAPTURE_UNKNOWN:
            emit errorMessage(tr("Could not determine the state of the Capture module"));
            break;
        case CAPTURE_IDLE:
        case CAPTURE_COMPLETE:
        case CAPTURE_ABORTED:
            stopKStars();
            break;
        default:
            emit errorMessage(tr("Capture module is in use"));
        };
    } else {
        emit errorMessage(tr("Could not determine the state of the Capture module"));
    }
}

// Shutdown KStars
void kstarsinterface::stopKStars()
{
    // Disconnect INDI, stop Ekos
    QDBusInterface ekInterface(serviceName, pathEkos, EkosInterface);
    if (ekInterface.isValid()) {
        QDBusMessage message = ekInterface.call("disconnectDevices");
        QDBusMessage message2 = ekInterface.call("stop");
    }

    // Close Ekos
    QDBusInterface showEkInterface(serviceName, showEkPath, qActionInterface);
    if (showEkInterface.isValid()) {
        QDBusMessage message = showEkInterface.call("trigger");
    }

    // Quit KStars
    QDBusInterface interface(serviceName, pathKStars, ksmwInterface);
    if (interface.isValid()) {
        QDBusMessage message = interface.call("activateAction", "quit");
        QList<QVariant> args = message.arguments();
        if ((args.count() == 1) && args.at(0).toBool()) {
            if (args.at(0).toBool() == true) {
                emit stoppedKS();
                haveShutdownKStars = true;
            } else {
                emit errorMessage(tr("Could not stop KStars"));
            }
        }
    }
}

// Restart KStars on app.aboutToQuit
// Note: we can't use QTimer::singleshot for the delays here as the event queue
// is no longer accepting new calls
void kstarsinterface::restartKStars()
{
    if (haveShutdownKStars) {
        QProcess m_kstars;
        QString prog = "kstars";
        QStringList args;

        // Start KStars
        m_kstars.startDetached(prog, args);

        // Open Ekos
        QThread::sleep(3);
        QDBusInterface showEkInterface(serviceName, showEkPath, qActionInterface);
        if (showEkInterface.isValid()) {
            QDBusMessage message = showEkInterface.call("trigger");
        }
        // Start INDI
        QThread::sleep(1);
        QDBusInterface ekInterface(serviceName, pathEkos, EkosInterface);
        if (ekInterface.isValid()) {
            QDBusMessage message = ekInterface.call("start");
        }
    }
}

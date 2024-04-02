#include "kstarsinterface.h"
#include "kstarsDBusConf.h"

#include <QtDBus>

kstarsinterface::kstarsinterface(QObject *parent)
{
    // Create a long term DBus Interface to monitor status signals
    QDBusInterface *monInterface = new QDBusInterface(serviceName, pathEkos, EkosInterface, QDBusConnection::sessionBus(), this);
    if (monInterface->isValid()) {
        connect(monInterface, SIGNAL(pluginStatusChanged(int)), this, SLOT(receiverStatusChanged(int)));
        capJobCount = new int(0);
    }
}

// Ephemeral test for usable DBus
void kstarsinterface::dbusAccessing()
{
    if (QDBusConnection::sessionBus().isConnected()) {
        kstarsAccessing();
    } else {
        emit errorMessage("Can not connect to DBus session interface, is the DBus deamon running?");
    }
}

// Ephemeral test for running KStars
void kstarsinterface::kstarsAccessing()
{
    QDBusInterface interface(serviceName, "/", ksInterface, bus, this);
    if (interface.isValid()) {
        schedulerChecking();
    } else {
        emit errorMessage("Can not connect to KStars DBus interface, is KStars running?");
    }
}

// Ephemeral test for idle Ekos Scheduler
void kstarsinterface::schedulerChecking()
{
    SchedulerState m_state = SCHEDULER_UNKNOWN;
    QDBusInterface interface(serviceName, pathScheduler);
    if (interface.isValid()) {
        m_state = static_cast<SchedulerState>(interface.property("status").toInt());
        switch (m_state) {
        case SCHEDULER_UNKNOWN:
            emit errorMessage("Could not determine the state of the Scheduler");
            break;
        case SCHEDULER_IDLE:
        case SCHEDULER_PAUSED:
        case SCHEDULER_ABORTED:
            captureChecking();
            break;
        default:
            emit errorMessage("Scheduler is in use");
        };
    } else {
        emit errorMessage("Could not determine the state of the Scheduler");
    }
}

// Ephemeral test for idle Ekos Capture Module
void kstarsinterface::captureChecking()
{
    CaptureState m_state = CAPTURE_UNKNOWN;
    QDBusInterface interface(serviceName, pathCapture);
    if (interface.isValid()) {
        m_state = static_cast<CaptureState>(interface.property("status").toInt());
        switch (m_state)
        {
        case CAPTURE_UNKNOWN:
            emit errorMessage("Could not determine the state of the Capture module");
            break;
        case CAPTURE_IDLE:
        case CAPTURE_COMPLETE:
        case CAPTURE_ABORTED:
            emit captureIdle();
            break;
        default:
            emit errorMessage("Capture module is in use");
        };
    } else {
        emit errorMessage("Could not determine the state of the Capture module");
    }
}

// Handle Ekos status changes
void kstarsinterface::receiverStatusChanged(int status)
{
    if (status == PLUGIN_STOP_REQUESTED) {
        emit stopSession();
    }
}

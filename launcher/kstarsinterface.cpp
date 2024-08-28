#include "kstarsinterface.h"
#include "kstarsDBusConf.h"
#include "ekosStatus.h"

kstarsinterface::kstarsinterface(QObject *parent)
{
    /* Watch for the extensionStatusChanged DBus signal,
     * this is emitted when the extension start/stop button is clicked in Ekos.
     * Note that the DBus message attributes can not be discovered until runtime,
     * therefore we have to use the 'old' string connect syntax and can not call
     * a lambda.
     */
    bus.connect(serviceName, QString(), EkosInterface, "extensionStatusChanged", this, SLOT (receiverStatusChanged(QDBusMessage)));
}

// Ephemeral test for usable DBus
bool kstarsinterface::checkDBus()
{
    if (QDBusConnection::sessionBus().isConnected()) return true;
    else return false;
}

// Ephemeral test for running KStars
bool kstarsinterface::checkKStarsService()
{
    QDBusInterface interface(serviceName, "/", ksInterface, bus, this);
    if (interface.isValid()) return true;
    else return false;
}

// Ephemeral test for idle Ekos Scheduler
SchedulerState kstarsinterface::checkSchedulerStatus()
{
    SchedulerState m_state = SCHEDULER_UNKNOWN;
    QDBusInterface interface(serviceName, pathScheduler);
    if (interface.isValid()) {
        m_state = static_cast<SchedulerState>(interface.property("status").toInt());
    }
    return m_state;
}

// Ephemeral test for idle Ekos Capture Module
CaptureState kstarsinterface::checkCaptureStatus()
{
    CaptureState m_state = CAPTURE_UNKNOWN;
    QDBusInterface interface(serviceName, pathCapture);
    if (interface.isValid()) {
        m_state = static_cast<CaptureState>(interface.property("status").toInt());
    }
    return m_state;
}

// Get the current Ekos Capture Module camera name
void kstarsinterface::getCamera()
{
    QDBusInterface interface(serviceName, pathCapture);
    if (interface.isValid()) {
        cameraName = static_cast<QString>(interface.property("camera").toString());
    }
}

/* Handle Ekos request to stop extension
 * Note the QDBusMessage parameter exists only to match the signature of the DBus
 * extensionStatusChanged signal, otherwise the connection doesn't work
 */

void kstarsinterface::receiverStatusChanged(QDBusMessage message)
{
    Q_UNUSED(message);
    // For this example we're only looking for an instruction to stop
    QDBusInterface interface(serviceName, pathEkos);
    if (interface.isValid()) {
        extensionState m_state = static_cast<extensionState>(interface.property("extensionStatus").toInt());
        if (m_state == EXTENSION_STOP_REQUESTED) {
            emit exitRequested();
        }
    }
}

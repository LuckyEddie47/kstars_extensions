#include "kstarsinterface.h"
#include "kstarsDBusConf.h"
#include "ekosStatus.h"

#include <QtDBus>

kstarsinterface::kstarsinterface(QObject *parent)
{
    /* Watch for the extensionStatusChanged DBus signal,
     * this is emitted when the extension start/stop button is clicked in Ekos.
     * Note that the DBus message attributes can not be discovered until runtime,
     * therefore we have to use the 'old' string connect syntax and can not call
     * a lambda.
     */
    bus.connect(serviceName, QString(), EkosInterface, "extensionStatusChanged", this, SLOT (receiverStatusChanged(bool)));
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

// Disconnect the INDI driver for the camera named in the Ekos Capture Module
bool kstarsinterface::disconnectCamera()
{
    bool result = false;

    getCamera();

    QDBusInterface interface(serviceName, pathINDIgeneric, introspectable);
    QDBusMessage message = interface.call("Introspect");
    QList<QVariant> args = message.arguments();
    QStringList messageParts = args[0].toString().split("\n");
    QStringList nodes;
    foreach (QString part, messageParts) {
        if (part.contains("node name")) {
            int lchop = (part.indexOf('"') + 1);
            int rchop = part.lastIndexOf('"');
            rchop -= lchop;
            nodes.append(part.mid(lchop, rchop));
        }
    }
    foreach (QString node, nodes) {
        QString devPath = pathINDIgeneric;
        devPath.append("/").append(node);
        QDBusInterface interfaceDev(serviceName, devPath);
        QString nodeName = static_cast<QString>(interfaceDev.property("name").toString());
        if (nodeName == cameraName) {
            cameraInterface = interfaceDev.path();
            QDBusMessage messageDisconnect = interfaceDev.call("Disconnect");
            result = true;
            break;
        }
    }
    return result;
}

// Reconnect the previously disconnected camera INDI driver
void kstarsinterface::reconnectCamera() {
    if (cameraName != "") {
        QDBusInterface interfaceCam(serviceName, cameraInterface);
        QDBusMessage messageResconnect = interfaceCam.call("Connect");
    }
}

/* Handle Ekos request to stop extension
 * Note the bool parameter exists only to match the signature of the DBus
 * extensionStatusChanged signal, otherwise the connection doesn't work
 */

void kstarsinterface::receiverStatusChanged(bool status)
{
    Q_UNUSED(status);
    // For this example we're only looking for an instruction to stop
    QDBusInterface interface(serviceName, pathEkos);
    if (interface.isValid()) {
        extensionState m_state = static_cast<extensionState>(interface.property("extensionStatus").toInt());
        if (m_state == EXTENSION_STOP_REQUESTED) {
            emit exitRequested();
        }
    }
}

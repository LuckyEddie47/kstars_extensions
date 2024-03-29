#include "kstarsinterface.h"
#include "kstarsDBusConf.h"

#include <QtDBus>

kstarsinterface::kstarsinterface(QObject *parent)
{
    // Create a long term DBus Interface to monitor status signals
    QDBusInterface *monInterface = new QDBusInterface(serviceName, pathEkos, EkosInterface, QDBusConnection::sessionBus(), this);
    if (monInterface->isValid()) {
        connect(monInterface, SIGNAL(pluginStatusChanged(int)), this, SLOT(receiverStatusChanged(int)));
    }
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

// Handle Ekos status changes
void kstarsinterface::receiverStatusChanged(pluginState status)
{
    if (status == PLUGIN_STOP_REQUESTED) {
        emit stopSession();
    }
}

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

// Setup the Ekos Manager preview to take images from passed file rather than the Capture Module
void kstarsinterface::setFITSfromFile(bool previewFromFile)
{
    QDBusInterface interfaceEkos(serviceName, pathEkos, EkosInterface);
    QDBusMessage messageOpen = interfaceEkos.call("setFITSfromFile", previewFromFile);
}

// Pass a filename to the Ekos Manager preview
void kstarsinterface::openFITSfile(const QString &filePath)
{
    QDBusInterface interfaceEkos(serviceName, pathEkos, EkosInterface);
    QDBusMessage messageOpen = interfaceEkos.call("previewFile", filePath);
}

// Handle Ekos status changes
void kstarsinterface::receiverStatusChanged(pluginState status)
{
    if (status == PLUGIN_STOP_REQUESTED) {
        emit stopSession();
    }
}

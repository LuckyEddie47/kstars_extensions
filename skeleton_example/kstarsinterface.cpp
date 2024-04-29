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

bool kstarsinterface::kstarsStateIsValid()
{
    bool isValid = false;

    if (bus.isConnected()) {
        QDBusInterface m_ksInterface(serviceName, "/", ksInterface, bus, this);
        if (m_ksInterface.isValid()) {
            isValid = true;
        } else {
            emit errorMessage("Can't access KStars over DBus.");
        }
    } else {
        emit errorMessage("Can't access DBus sessionBus");
    }

    return isValid;
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

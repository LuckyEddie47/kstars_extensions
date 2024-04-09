#include "kstarsinterface.h"
#include "kstarsDBusConf.h"

//#include <QtDBus>

kstarsinterface::kstarsinterface(QObject *parent)
{
    // Create a long term DBus Interface to monitor status signals from KStars
    QDBusInterface *monInterface = new QDBusInterface(serviceName, pathEkos, EkosInterface, QDBusConnection::sessionBus(), this);
    if (monInterface->isValid()) {

        // Accessing DBus directly means that the attributes of methods and properties can not be discovered at compile time,
        // so we have to use the older syntax for QOject::connect
 //       monInterface->connection().connect(QString(), QString(), EkosInterface, "extensionStatusChanged", this, SLOT(receiverStatusChanged(bool)));

        connect(monInterface, SIGNAL(extensionStatusChanged(bool)), this, SLOT(receiverStatusChanged(bool)));
//        connect(monInterface, SIGNAL(extensionStatusChanged(bool)), this, [this] {
//            receiverStatusChanged(true);
//        });
    }

//    bus.registerObject("/", this);
//    bus.connect(serviceName, QString(), EkosInterface, "extensionStatusChanged", this, SLOT (receiverStatusChanged(QDBusMessage)));

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

// Handle Ekos status changes
void kstarsinterface::receiverStatusChanged(bool status)
{
    Q_UNUSED(status);
    // For this example we're only looking for an instruction to stop
    QDBusInterface interface(serviceName, pathEkos, EkosInterface);
    if (interface.isValid()) {
        QDBusMessage m_message = interface.call("extensionStatus");
      QList<QVariant> args = m_message.arguments();
        if ((args.count() == 1) && args.at(0).toInt()) {
            if (EXTENSION_STOP_REQUESTED == args.at(0).toInt())
            {
                emit exitRequested();
            }
        }
    }
}

// Handle Ekos status changes
//void kstarsinterface::receiverStatusChanged()
//{
//    // For this example we're only looking for an instruction to stop
//    QDBusInterface interface(serviceName, pathEkos, EkosInterface);
//    if (interface.isValid()) {
//        QDBusMessage message = interface.call("extensionStatus");
//        QList<QVariant> args = message.arguments();
//        if ((args.count() == 1) && args.at(0).toInt()) {
//            if (EXTENSION_STOP_REQUESTED == args.at(0).toInt())
//            {
//                emit exitRequested();
//            }
//        }
//    }
//}

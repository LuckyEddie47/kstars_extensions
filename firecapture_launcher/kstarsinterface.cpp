#include "kstarsinterface.h"
#include "kstarsDBusConf.h"

#include <QtDBus>
#include <QDebug>

kstarsinterface::kstarsinterface(QObject *parent)
{
    QDBusInterface *monInterface = new QDBusInterface(serviceName, pathEkos, EkosInterface, QDBusConnection::sessionBus(), this);
    if (monInterface->isValid()) {
        connect(monInterface, SIGNAL(EAAStatusChanged(bool)), this, SLOT(receiverStatusChanged(bool)));
    }
}

bool kstarsinterface::checkDBus()
{
    if (QDBusConnection::sessionBus().isConnected()) return true;
    else return false;
}

bool kstarsinterface::checkKStarsService()
{
    QDBusInterface interface(serviceName, "/", ksInterface, bus, this);
    if (interface.isValid()) return true;
    else return false;
}

SchedulerState kstarsinterface::checkSchedulerStatus()
{
    SchedulerState m_state = SCHEDULER_UNKNOWN;
    QDBusInterface interface(serviceName, pathScheduler);
    if (interface.isValid()) {
        m_state = static_cast<SchedulerState>(interface.property("status").toInt());
    }
    return m_state;
}

CaptureState kstarsinterface::checkCaptureStatus()
{
    CaptureState m_state = CAPTURE_UNKNOWN;
    QDBusInterface interface(serviceName, pathCapture);
    if (interface.isValid()) {
        m_state = static_cast<CaptureState>(interface.property("status").toInt());
    }
    return m_state;
}

void kstarsinterface::getCamera()
{
    QDBusInterface interface(serviceName, pathCapture);
    if (interface.isValid()) {
        cameraName = static_cast<QString>(interface.property("camera").toString());
    }
}

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

void kstarsinterface::reconnectCamera() {
    if (cameraName != "") {
        QDBusInterface interfaceCam(serviceName, cameraInterface);
        QDBusMessage messageResconnect = interfaceCam.call("Connect");
    }
}

void kstarsinterface::setFITSfromFile(bool previewFromFile)
{
    QDBusInterface interfaceEkos(serviceName, pathEkos, EkosInterface);
    QDBusMessage messageOpen = interfaceEkos.call("setFITSfromFile", previewFromFile);
    QList<QVariant> args = messageOpen.arguments();
}

void kstarsinterface::openFITSfile(const QString &filePath)
{
    QDBusInterface interfaceEkos(serviceName, pathEkos, EkosInterface);
    QDBusMessage messageOpen = interfaceEkos.call("previewFile", filePath);
    QList<QVariant> args = messageOpen.arguments();
}

void kstarsinterface::receiverStatusChanged(bool status)
{
    if (status == false) {
        emit stopEAAsession();
    }
}


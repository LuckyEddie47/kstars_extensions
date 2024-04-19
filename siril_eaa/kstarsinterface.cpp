#include "kstarsinterface.h"
#include "kstarsDBusConf.h"
#include "ekosStatus.h"
#include "sirilinterface.h"

#include <QtDBus>

kstarsinterface::kstarsinterface(QObject *parent)
{
    /* Watch for the extensionStatusChanged DBus signal,
     * this is emitted when the extension start/stop button is clicked in Ekos.
     * Note that the DBus message attributes can not be discovered until runtime,
     * therefore we have to use the 'old' string connect syntax and can not call
     * a lambda.
     */
    bus.connect(serviceName, QString(), EkosInterface, "extensionStatusChanged", this, SLOT (receiverStatusChanged(QDBusMessage)));
    bus.connect(serviceName, QString(), captureInterface, "captureComplete", this, SLOT (handleCapturedImage(QDBusMessage)));
}

// Ephemeral test for usable DBus
void kstarsinterface::dbusAccessing()
{
    if (QDBusConnection::sessionBus().isConnected()) {
        emit dbusAccessible();
    } else {
        emit errorMessage(tr("Can not connect to DBus session interface, is the DBus deamon running?"));
    }
}

// Ephemeral test for running KStars
void kstarsinterface::kstarsAccessing()
{
    QDBusInterface interface(serviceName, "/", ksInterface, bus, this);
    if (interface.isValid()) {
        emit kstarsAccessible();
    } else {
        emit errorMessage(tr("Can not connect to KStars DBus interface, is KStars running?"));
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
            emit errorMessage(tr("Could not determine the state of the Scheduler"));
            break;
        case SCHEDULER_IDLE:
        case SCHEDULER_PAUSED:
        case SCHEDULER_ABORTED:
            emit schedulerIdle();
            break;
        default:
            emit errorMessage(tr("Scheduler is in use"));
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
            emit errorMessage(tr("Could not determine the state of the Capture module"));
            break;
        case CAPTURE_IDLE:
        case CAPTURE_COMPLETE:
        case CAPTURE_ABORTED:
            emit captureIdle();
            break;
        default:
            emit errorMessage(tr("Capture module is in use"));
        };
    } else {
        emit errorMessage(tr("Could not determine the state of the Capture module"));
    }
}

// Setup the Ekos Manager preview to take images from passed file rather than the Capture Module
void kstarsinterface::setFITSfromFile(bool previewFromFile)
{
    QDBusInterface interface(serviceName, pathEkos, EkosInterface);
    if (interface.isValid()) {
        QDBusMessage message = interface.call("setFITSfromFile", previewFromFile);
    } else if(previewFromFile) {
        emit errorMessage(tr("Could not set Ekos Preview mode"));
    }
}

// Pass a filename to the Ekos Manager preview
void kstarsinterface::openFITSfile(const QString &filePath)
{
    QDBusInterface interface(serviceName, pathEkos, EkosInterface);
    if (interface.isValid()) {
        QDBusMessage message = interface.call("previewFile", filePath);
    } else {
        emit errorMessage(tr("Could not send Stack to Ekos"));
    }
}

//void kstarsinterface::receiverStatusChanged(bool status)
void kstarsinterface::receiverStatusChanged(QDBusMessage message)
{
    Q_UNUSED(message);
    // We're only looking for an instruction to stop
    QDBusInterface interface(serviceName, pathEkos);
    if (interface.isValid()) {
        extensionState m_state = static_cast<extensionState>(interface.property("extensionStatus").toInt());
        if (m_state == EXTENSION_STOP_REQUESTED) {
            emit stopSession();
        }
    }
}

// Get the count of jobs in Capture
void kstarsinterface::captureCheckingNoJobs()
{
    QDBusInterface interface(serviceName, pathCapture);
    if (interface.isValid()) {
        QDBusMessage message = interface.call("getJobCount");
        QList<QVariant> args = message.arguments();
        if (args.count() == 1) { //&& args.at(0).toInt()) {
            if (args.at(0).toInt() == 0) {
                emit captureNoJobs();
            } else {
                emit errorMessage(tr("Capture has jobs"));
            }
        } else {
            emit errorMessage(tr("Could not get Capture job count"));
        }
    } else {
        emit errorMessage(tr("Could not get Capture job count"));
    }
}

// Get the file format string and check
void kstarsinterface::captureGettingFileFormat()
{
    QDBusInterface interface(serviceName, pathCapture);
    if (interface.isValid()) {
        QDBusMessage message = interface.call("getJobPlaceholderFormat");
        QList<QVariant> args = message.arguments();
        if (args.count() == 1) {
            QString format = args.at(0).toString();
            format = format.left(format.lastIndexOf("/"));
            if (!format.contains("%D") && !format.contains("%C") && !format.contains("%P")) {
                emit captureFormatOkay();
            } else {
                emit errorMessage(tr("Placeholder format contains variable path tags"));
            }
        } else {
            emit errorMessage(tr("Could not get placeholder format"));
        }
    } else {
        emit errorMessage(tr("Could not get placeholder format"));
    }
}

// Get the capture job file path
void kstarsinterface::captureGettingFilePath()
{
    QDBusInterface interface(serviceName, pathCapture);
    if (interface.isValid()) {
        QDBusMessage message = interface.call("getJobPreviewFileName");
        QList<QVariant> args = message.arguments();
        if (args.count() == 1) {
            QString path = args.at(0).toString();
            path = path.left(path.lastIndexOf("/"));
            QDir m_dir(path);
            // If the dir already exists does nothing, otherwise creates with parents
            m_dir.mkpath(".");
            jobPath = path;
            emit captureFilePath(path);
            emit readCaptureFilePath();
        } else {
            emit errorMessage(tr("Could not get image filepath"));
        }
    } else {
        emit errorMessage(tr("Could not get image filepath"));
    }
}

void kstarsinterface::captureSetttingDisplayExternal()
{
    setFITSfromFile(true);
    emit captureDisplaySet();
}

// Tell Capture to start the job
void kstarsinterface::captureJobRunning()
{
    QDBusInterface interface(serviceName, pathCapture);
    if (interface.isValid()) {
        QDBusMessage message = interface.call("start");
    } else {
        emit errorMessage(tr("Could not start the Capture job"));
    }
}

void kstarsinterface::handleCapturedImage(QDBusMessage message)
{
    const QDBusArgument &dbusArg = message.arguments().at(0).value<QDBusArgument>();
    QMap<QString, QVariant> map;
    dbusArg >> map;
    emit newCaptureImage(map.value("filename").toString());
    emit captureImageTaken();
}

void kstarsinterface::sendStacktoEkos()
{
    openFITSfile(QString("%1%2%3").arg(jobPath, "/", SirilStackName));
    emit readyForNext();
}

// Tell Capture to stop, clear the sequence queue of out preview job and reset the preview window mode
void kstarsinterface::captureStopAndReset()
{
    QDBusInterface interface(serviceName, pathCapture);
    if (interface.isValid()) {
        QDBusMessage message = interface.call("stop");
        QDBusMessage message2 = interface.call("clearSequenceQueue");
    }
    setFITSfromFile(false);
}

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
        emit dbusAccessible();
    } else {
        emit errorMessage("Can not connect to DBus session interface, is the DBus deamon running?");
    }
}

// Ephemeral test for running KStars
void kstarsinterface::kstarsAccessing()
{
    QDBusInterface interface(serviceName, "/", ksInterface, bus, this);
    if (interface.isValid()) {
        emit kstarsAccessible();
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
            emit schedulerIdle();
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

// Setup the Ekos Manager preview to take images from passed file rather than the Capture Module
void kstarsinterface::setFITSfromFile(bool previewFromFile)
{
    QDBusInterface interface(serviceName, pathEkos, EkosInterface);
    if (interface.isValid()) {
        QDBusMessage message = interface.call("setFITSfromFile", previewFromFile);
    } else {
        emit errorMessage("Could not set Ekos Preview mode");
    }
}

// Pass a filename to the Ekos Manager preview
void kstarsinterface::openFITSfile(const QString &filePath)
{
    QDBusInterface interface(serviceName, pathEkos, EkosInterface);
    if (interface.isValid()) {
        QDBusMessage message = interface.call("previewFile", filePath);
    } else {
        emit errorMessage("Could not send Stack to Ekos");
    }
}

// Handle Ekos status changes
void kstarsinterface::receiverStatusChanged(int status)
{
    if (status == PLUGIN_STOP_REQUESTED) {
        emit stopSession();
    }
}

// Get the count of jobs in Capture
void kstarsinterface::captureCheckingNoJobs()
{
    QDBusInterface interface(serviceName, pathCapture);
    if (interface.isValid()) {
        QDBusMessage message = interface.call("getJobCount");
        QList<QVariant> args = message.arguments();
        if ((args.count() == 1) && args.at(0).toInt()) {
            *capJobCount = args.at(0).toInt();
        }
        if (*capJobCount == 0) {
            emit captureNoJobs();
        } else {
            emit errorMessage("Capture has jobs");
        }
    } else {
        emit errorMessage("Could not get Capture job count");
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
            format.left(format.lastIndexOf("/"));
            if (!format.contains("%D") && !format.contains("%C") && !format.contains("%P")) {
                emit captureFormatOkay();
            } else {
                emit errorMessage("Placeholder format contains variable path tags");
            }
        } else {
            emit errorMessage("Could not get placeholder format");
        }
    } else {
        emit errorMessage("Could not get placeholder format");
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
            emit captureFilePath(path);
            emit readCaptureFilePath();
        } else {
            emit errorMessage("Could not get image filepath");
        }
    } else {
        emit errorMessage("Could not get image filepath");
    }
}

// Tell Capture to start the job
void kstarsinterface::captureJobRunning()
{
    QDBusInterface interface(serviceName, pathCapture);
    if (interface.isValid()) {
        QDBusMessage message = interface.call("start");
    } else {
        emit errorMessage("Could not start the Capture job");
    }
}

// Tell Capture to stop and reset preview window mode
void kstarsinterface::captureStopAndReset()
{
    QDBusInterface interface(serviceName, pathCapture);
    if (interface.isValid()) {
        QDBusMessage message = interface.call("stop");
    } else {
        emit errorMessage("Could not stop the Capture job");
    }
    setFITSfromFile(false);
}

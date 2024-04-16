#include "sirilinterface.h"

#include "fcntl.h"
#include <QDebug>
#include <QTimer>
#include <QThread>

sirilinterface::sirilinterface(QObject *parent)
    : QObject{parent}
{}

// Set the path to Siril (from configuration file checker)
void sirilinterface::setSirilPath(QString path)
{
    sirilPath = path;
}

// Get the working directory
const QString sirilinterface::getWD()
{
    return workingDir;
}

// Set the working directory for Siril
void sirilinterface::setWD(QString path)
{
    workingDir = path;
}

// Set the path to a dark calibration frame
void sirilinterface::setDarkPath(QString path)
{
    darkPath = path;
}

// Set the path to a flat calibration frame
void sirilinterface::setFlatPath(QString path)
{
    flatPath = path;
}

// Set the registration mode
void sirilinterface::setRegistrationMode(QString mode)
{
    registrationMode = mode;
}

// Launch Siril
void sirilinterface::startSiril()
{
    if (sirilPath != "") {
        // Kill any existing running instances
        QProcess killer;
        QString killerProc("pkill");
        QStringList killerArgs = QStringList() << "-f" << "/siril";
        killer.start(killerProc, killerArgs);
        killer.waitForReadyRead(1000);
        killer.terminate();
        killer.waitForFinished(1000);
        killer.kill();

        QString wd = sirilPath.left(sirilPath.lastIndexOf("/"));
        QStringList arguments;
        arguments << "-p";

        // Need a 5 second delay for first run of Siril to create command pipes
        connect(&programProcess, &QProcess::started, this, [=] (){
            QTimer::singleShot(5000, this, [this] {
                emit sirilStarted();
            });
        });
        connect(&programProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this,
                [=](int exitCode, QProcess::ExitStatus exitStatus) {
                    emit sirilFinished();
                });

        programProcess.setWorkingDirectory(wd);
        programProcess.setProcessChannelMode(QProcess::ForwardedChannels);
        programProcess.start(sirilPath, arguments);

    } else {
        emit errorMessage("SirilPath has not been set");
    }
}

// Setup Siril read pipe
void sirilinterface::connectSiril()
{
    bool okayToProceed = false;

    messagePipe = new QFile(sirilMessages);
    if (messagePipe->exists()) {
        okayToProceed = true;
    } else {
        emit errorMessage(QString("Input file: %1 does not exist").arg(sirilMessages));
        okayToProceed = false;
    }

    if (okayToProceed) {
        if (!messagePipe->open(QFile::ReadOnly | QFile::Unbuffered)) {
            emit errorMessage(QString("Can not open message pipe %1").arg(sirilMessages));
            okayToProceed = false;
        }
    }

    if (okayToProceed) {
        fd = messagePipe->handle();

        flags = fcntl(fd, F_GETFL, 0);
        if (flags == -1) {
            emit errorMessage("Can not access message pipe flags");
            okayToProceed = false;
        }
    }

    if (okayToProceed) {
        flags = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
        if (flags == -1) {
            emit errorMessage("Can not set message pipe flags");
            okayToProceed = false;
        }
    }

    if (okayToProceed) {
        notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
        connect(notifier, &QSocketNotifier::activated, this, &sirilinterface::readMessage);
        QTimer::singleShot(1000, this, [this] {
            emit sirilConnected();
        });
    }
}

// Set Sirils working directory
void sirilinterface::setSirilWD()
{
    if (workingDir != "") {
        sendSirilCommand(QString("cd ").append(workingDir));
    } else {
        emit errorMessage("Working directory has not been set");
    }
}

// Set Siril in LiveStacking mode
void sirilinterface::setSirilLS()
{
    QStringList args;
    if (darkPath != "") {
        args.append(QString(" -dark=%1").arg(darkPath));
    }
    if (flatPath != "") {
        args.append(QString(" -flat=%1").arg(flatPath));
    }
    if (registrationMode == "rotate") {
        args.append(" -rotate");
    }
    QString lscommand("start_ls");
    if (args.count() > 0 ) {
        foreach (QString arg, args) {
            lscommand.append(arg);
        }
    }
    sendSirilCommand(lscommand);
}

// Close Siril
void sirilinterface::stopProgram()
{
    programProcess.close();
}

// Read message from Siril
void sirilinterface::readMessage()
{
    char buffer[4096];
    QByteArray messageBA;
    while (messagePipe->read(buffer, sizeof(buffer)) > 0) {
        messageBA.append(buffer);
    }
    if (!messageBA.isEmpty()) {
        if (messageBA.contains("\n")) {
            messageBA.truncate(messageBA.lastIndexOf("\n"));
        }
        if (QString(messageBA) == "ready") {
            emit sirilReady();
        } else if (QString(messageBA).contains("status: success cd")) {
            emit sirilCdSuccess();
        } else if (QString(messageBA).contains("status: success start_ls")) {
            emit sirilLsStarted();
        } else if ((QString(messageBA).contains("log: Waiting for second image")) ||
                   (QString(messageBA).contains("log: Sequence processing partially succeeded")) ||
                   (QString(messageBA).contains("log: Stacked image"))) {
            emit sirilStackReady();
        }

        emit sirilMessage (QString(messageBA));
    }
}

// Send command to Siril
void sirilinterface::sendSirilCommand(QString command)
{
    QFile commandPipe(sirilCommands);
    if (commandPipe.open(QIODevice::WriteOnly)) {
        commandPipe.setTextModeEnabled(true);
        QTextStream commandTS(&commandPipe);
        commandTS << command << Qt::endl;
        commandPipe.close();
    }
}

void sirilinterface::newImageFromKStars(const QString &filePath)
{
    newImagePath = filePath;
}

void sirilinterface::sendImageToSiril()
{
    if (newImagePath != "") {
        sendSirilCommand(QString("livestack %1").arg(newImagePath));
    }
}

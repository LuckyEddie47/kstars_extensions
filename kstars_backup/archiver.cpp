// Provides the interface to the archive and filesystem
// Uses tar, df, and du commands
// All command options must be short form for MacOS compatibility

#include "archiver.h"

#include <QDateTime>

archiver::archiver(QObject *parent)
    : QObject{parent}
{
    m_writer = new QProcess(this);
    m_reader = new QProcess(this);
    m_sizer = new QProcess(this);
    m_free = new QProcess(this);
    m_used = new QProcess(this);
    m_extractor = new QProcess(this);
}

void archiver::read()
{
    QString inFile = archivePath;
    QString prog = "tar";
    QStringList args;
    args << "-t" << "-z" << "-f" << inFile;
    m_reader->setProcessChannelMode(QProcess::MergedChannels);

    connect(m_reader, &QProcess::readyReadStandardOutput, this, [=] {
        QString returnText = m_reader->readAllStandardOutput();
        if (returnText != "") {
            outputLines = returnText.split("\n", Qt::SkipEmptyParts);
            QStringList returnLines;
            if (sets.count() == 0) {
                sets << QString(outputLines.at(0)).prepend("/");
            }
            foreach (QString line, outputLines) {
                line = line.prepend("/");
                bool found = false;
                foreach (QString set, sets) {
                    if (line.startsWith(set)) {
                        found = true;
                    }
                }
                if (!found) {
                    QString tempLine = line;
                    if (tempLine.count("/") > 1 && !tempLine.endsWith("/")) {
                        tempLine = tempLine.left(tempLine.lastIndexOf("/"));
                    }
                    sets << tempLine;
                }
            }
        }
    });

    connect(m_reader, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this,
            [=](int exitCode, QProcess::ExitStatus exitStatus) {
        emit readSets(sets);
        getSizes();
    });

    m_reader->start(prog, args);
}

void archiver::getSizes()
{
    QString inFile = archivePath;
    QString prog = "tar";
    QStringList args;
    args << "-t" << "-v" << "-f" << inFile;
    m_sizer->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_sizer, &QProcess::readyReadStandardOutput, this, [=] {
        QString returnText = m_sizer->readAllStandardOutput();
        if (returnText != "") {
            outputLines = returnText.split("\n", Qt::SkipEmptyParts);
            QStringList sizes;
            foreach (QString line, outputLines) {
                QStringList splits = line.split(QRegExp("\\s+"), Qt::SkipEmptyParts);
                sizes << splits.at(2);
            }
            foreach (QString size, sizes) {
                sizeLines << size;
            }
        }
    });

    connect(m_sizer, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this,
            [=](int exitCode, QProcess::ExitStatus exitStatus) {
                totalSize();
            });

    m_sizer->start(prog, args);
}

void archiver::totalSize()
{
    total = 0;
    foreach (QString size , sizeLines) {
        total += size.toULong();
    }
    emit archiveSize(total);
}

void archiver::getDestinationSpace(const QString &path)
{
    QString prog = "df";
    QStringList args;
    args << path;
    m_free->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_free, &QProcess::readyReadStandardOutput, this, [=] {
        QString returnText = m_free->readAllStandardOutput();
        if (returnText != "") {
            outputLines = returnText.split("\n", Qt::SkipEmptyParts);
            if (outputLines.count() == 2) {
                QStringList splits = outputLines.at(1).split(QRegExp("\\s+"), Qt::SkipEmptyParts);
                QString freeSpace = splits.at(3);
                emit destinationSpace(freeSpace.toULong());
            }
        }
    });

    m_free->start(prog, args);
}

void archiver::getSourceSize(const QStringList &paths)
{
    QString prog = "du";
    QStringList args;
    args << "-s" << "-c" << paths;
    m_used->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_used, &QProcess::readyReadStandardOutput, this, [=] {
        QString returnText = m_used->readAllStandardOutput();
        if (returnText != "") {
            outputLines = returnText.split("\n", Qt::SkipEmptyParts);
            QStringList splits = outputLines.at(outputLines.count() - 1).split(QRegExp("\\s+"), Qt::SkipEmptyParts);
            QString usedSpace = splits.at(0);
            emit sourceSize(usedSpace.toULong());
        }
    });

    m_free->start(prog, args);
}

void archiver::write(const QStringList &files)
{
    QString outFile = archivePath.append("/").append(createArchiveName());
    QString prog = "tar";
    QStringList args;
    args << "-c" << "-z" << "-f" << outFile << files;
    m_writer->setProcessChannelMode(QProcess::ForwardedChannels);

    m_writer->start(prog, args);
}

void archiver::extract()
{

}

QString archiver::createArchiveName()
{
    QString hostName = QSysInfo::machineHostName();
    QString osVersion = QSysInfo::productVersion();
    QString architecture = QSysInfo::currentCpuArchitecture();
    QString kstarsVersion = "";
    QString now = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss");
    return QString("KStars backup V").append(kstarsVersion).append(" ")
                      .append(hostName).append(" ").append(osVersion).append(" ")
                      .append(architecture).append(" ").append(now).append(".tar.gz");
}

void archiver::setArchivePath(const QString &filepath)
{
    archivePath = filepath;
}

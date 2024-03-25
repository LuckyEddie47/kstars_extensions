#include "archiver.h"

#include <QDateTime>

archiver::archiver(QObject *parent)
    : QObject{parent}
{
    m_writer = new QProcess(this);
    m_reader = new QProcess(this);
    m_extractor = new QProcess(this);
}

void archiver::read()
{
    QString inFile = archivePath;
    QString prog = "tar";
    QStringList args;
    args << "--list" << "--gunzip" << "--file" << inFile;
    m_reader->setProcessChannelMode(QProcess::MergedChannels);

    connect(m_reader, &QProcess::readyReadStandardOutput, this, [=] {
        QString returnText = m_reader->readAllStandardOutput();
        if (returnText != "") {
            outputLines = returnText.split("\n", Qt::SkipEmptyParts);
            QStringList returnLines;
            if (sets.count() == 0) {
                sets << outputLines.at(0);
            }
            foreach (QString line, outputLines) {
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
    });

    m_reader->start(prog, args);
}

void archiver::write(const QStringList &files)
{
    QString outFile = archivePath.append("/").append(createArchiveName());
    QString prog = "tar";
    QStringList args;
    args << "--create" << "--gzip" << "--file" << outFile << files;
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

void archiver::setArchivePath(const QString filepath)
{
    archivePath = filepath;
}

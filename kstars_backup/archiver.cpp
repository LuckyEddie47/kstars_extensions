#include "archiver.h"

#include <QDateTime>

archiver::archiver(QObject *parent)
    : QObject{parent}
{
    m_archive = new QProcess();
}

QStringList archiver::read()
{
    QString inFile = archivePath;
    QString prog = "tar";
    QStringList args;
    args << "--list" << "gunzip" << "--file" << inFile;
    connect (m_archive, &QProcess::readyReadStandardOutput, this, [=] {
        QString returnText = m_archive->readAllStandardOutput();
        result = returnText.split("\n");
    });
    return result;
}

void archiver::write(const QStringList &files)
{
    QString outFile = archivePath.append("/").append(createArchiveName());
    QString prog = "tar";
    QStringList args;
    args << "--create" << "--gzip" << "--file" << outFile << files;
    m_archive->setProcessChannelMode(QProcess::ForwardedChannels);
    m_archive->start(prog, args);
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

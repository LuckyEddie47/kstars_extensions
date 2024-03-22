#include "archiver.h"

archiver::archiver(const QString &filename, QObject *parent)
    : QObject{parent}
{
    m_archive = new QProcess();
}

QStringList archiver::read()
{
    return QStringList() << "a" << "b";
}

void archiver::write(const QStringList &files)
{
    QString outFile("/tmp/test.tar.gz");
    QString prog = "tar";
    QStringList args;
    args << "--create" << "--gzip" << "--file" << outFile << files;
    m_archive->setProcessChannelMode(QProcess::ForwardedChannels);
    m_archive->start(prog, args);
}

void archiver::extract()
{

}

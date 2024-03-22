#include "archiver.h"

archiver::archiver(const QString &filename, QObject *parent)
    : QObject{parent}
{
    m_archive = new QuaZip(filename);
}

QStringList archiver::read()
{
    return QStringList() << "a" << "b";
}

void archiver::write(const QStringList &files)
{
    m_archive->open(QuaZip::mdCreate);
    QuaZipFile m_file(m_archive);
    m_file << QFile("/home/ed/");
}

void archiver::extract()
{

}

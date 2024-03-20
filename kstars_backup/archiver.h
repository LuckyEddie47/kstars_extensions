#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <QObject>
#include <k7zip.h>

class archiver : public QObject
{
    Q_OBJECT
public:
    explicit archiver(const QString &filename, QObject *parent = nullptr);
    QStringList read();
    void write(const QStringList &files);
    void extract();

signals:

private:
    K7Zip* m_archive;
};

#endif // ARCHIVER_H

#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <QObject>
#include <QProcess>

class archiver : public QObject
{
    Q_OBJECT
public:
    explicit archiver(const QString &filepath, QObject *parent = nullptr);
    QString createArchiveName();
    void setArchivePath();
    QStringList read();
    void write(const QStringList &files);
    void extract();

signals:

private:
    QProcess* m_archive;
 //   QString archiveName = "";
    QString archivePath = "";
};

#endif // ARCHIVER_H

#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <QObject>
#include <QProcess>

class archiver : public QObject
{
    Q_OBJECT
public:
    explicit archiver(QObject *parent = nullptr);
    QString createArchiveName();
    void setArchivePath(const QString filepath);
    QStringList read();
    void write(const QStringList &files);
    void extract();

signals:

private:
    QProcess* m_archive;
 //   QString archiveName = "";
    QString archivePath = "";
    QStringList result;
};

#endif // ARCHIVER_H

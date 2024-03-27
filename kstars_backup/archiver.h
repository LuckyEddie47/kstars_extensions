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
    void setArchivePath(const QString &filepath);
    void read();
    void write(const QStringList &files);
    void extract();
    void getSizes();
    void totalSize();
    void getDestinationSpace(const QString &path);

signals:
    void readSets(QStringList readSets);
    void archiveSize(ulong size);
    void destinationSpace(ulong size);

private:
    QProcess* m_writer;
    QProcess* m_reader;
    QProcess* m_sizer;
    QProcess* m_free;
    QProcess* m_extractor;

    QString archivePath = "";
    QStringList outputLines;
    QStringList sets;
    QStringList sizeLines;
    ulong total = 0;
    ulong space = 0;
};

#endif // ARCHIVER_H

#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <QObject>
#include <QProcess>

class archiver : public QObject
{
    Q_OBJECT
public:
    explicit archiver(const QString &ks_version, QObject *parent = nullptr);
    QString createArchiveName();
    void setArchivePath(const QString &filepath);
    void read();
    void write(const QStringList &paths);
    void extract();
    void getSizes();
    void totalSize();
    void getDestinationSpace(const QString &path);
    void getSourceSize(const QStringList &paths);

signals:
    void readSets(QStringList readSets);
    void archiveSize(ulong size);
    void destinationSpace(ulong size);
    void sourceSize(ulong size);
    void done();

private:
    QProcess* m_writer;
    QProcess* m_reader;
    QProcess* m_sizer;
    QProcess* m_free;
    QProcess* m_used;
    QProcess* m_extractor;
    QProcess* m_compressed;

    QString archivePath = "";
    QStringList outputLines;
    QStringList sets;
    QStringList sizeLines;
    ulong total = 0;
    ulong space = 0;
    QString kstars_version = "";
};

#endif // ARCHIVER_H

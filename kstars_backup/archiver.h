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
    void read();
    void write(const QStringList &files);
    void extract();

signals:
    void readSets(QStringList readSets);

private:
    QProcess* m_writer;
    QProcess* m_reader;
    QProcess* m_extractor;

    QString archivePath = "";
    QStringList outputLines;
    QStringList sets;
};

#endif // ARCHIVER_H

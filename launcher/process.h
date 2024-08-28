// Provides the interface to the external program for this plugin

#ifndef PROCESS_H
#define PROCESS_H

#include <QObject>
#include <QProcess>

class process : public QObject
{
    Q_OBJECT
public:
    explicit process(QObject *parent = nullptr);
    bool startScriptUpToDate(const QString &path);
    void startProgram(const QString &path);
    void stopProgram();

signals:
    void programFinished();
    void programStarted();

private:
    QProcess programProcess;
};

#endif // PROCESS_H

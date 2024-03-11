// Provides the interface to FireCapture for this plugin

#ifndef PROCESS_H
#define PROCESS_H

#include <QObject>
#include <QProcess>

class process : public QObject
{
    Q_OBJECT
public:
    explicit process(QObject *parent = nullptr);
    void startProgram(QString path);
    void stopProgram();

signals:
    void programFinished();
    void programStarted();

private:
    QProcess programProcess;
};

#endif // PROCESS_H

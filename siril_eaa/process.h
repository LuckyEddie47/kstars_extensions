// Provides the interface to Siril for this plugin

#ifndef PROCESS_H
#define PROCESS_H

#include <QObject>
#include <QProcess>
#include <QSocketNotifier>
#include <QFile>

class process : public QObject
{
    Q_OBJECT
public:
    explicit process(QObject *parent = nullptr);

public slots:
    void startProgram(QString path);
    void stopProgram();
    void programRunning();
    void sendSirilCommand(QString command);
//    QString livestackImage(QString filename);

signals:
    void programFinished();
    void programStarted();
    void sirilMessage(QString message);
    void processError(QString error);

private:
    bool startLivestacking();
    bool stopLivestacking();
    void readMessage();

    QProcess programProcess;
    QString sirilMessages = "/tmp/siril_command.out";
    QString sirilCommands = "/tmp/siril_command.in";
    QSocketNotifier* notifier;
    int fd;
    int flags;
    QFile* messagePipe;
};

#endif // PROCESS_H

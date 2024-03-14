#ifndef SIRILINTERFACE_H
#define SIRILINTERFACE_H

#include <QObject>
#include <QProcess>
#include <QSocketNotifier>
#include <QFile>

class sirilinterface : public QObject
{
    Q_OBJECT
public:
    explicit sirilinterface(QObject *parent = nullptr);

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

#endif // SIRILINTERFACE_H

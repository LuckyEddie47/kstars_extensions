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
    void setSirilPath(QString path);
    void startProgram();
    void stopProgram();
    void programRunning();
    void sendSirilCommand(QString command);
    //    QString livestackImage(QString filename);

signals:
    void programFinished();
    void programStarted();
    void sirilMessage(QString message);
    void processError(QString error);

    void errorMessage(QString errorDetail);

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
    QString sirilPath;
    QFile* messagePipe;
};

#endif // SIRILINTERFACE_H

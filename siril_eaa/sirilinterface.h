#ifndef SIRILINTERFACE_H
#define SIRILINTERFACE_H

#include <QObject>
#include <QProcess>
#include <QSocketNotifier>
#include <QFile>

/* Currently Siril always uses a fixed file name for the
 * live stacked output - likely to change in the future
 */
const QString SirilStackName = "live_stack_00001.fit";

class sirilinterface : public QObject
{
    Q_OBJECT
public:
    explicit sirilinterface(QObject *parent = nullptr);

public slots:
    void setSirilPath(QString path);
    const QString getWD();
    void setWD(QString workingDir);
    void setDarkPath(QString path);
    void setFlatPath(QString path);
    void setRegistrationMode(QString mode);

    void checkSiril();
    void startSiril();
    void connectSiril();
    void setSirilWD();
    void setSirilLS();
    void stopProgram();
    void sendSirilCommand(QString command);
    void newImageFromKStars(const QString &filePath);
    void sendImageToSiril();

signals:
    void sirilClosed();
    void sirilFinished();
    void sirilChecked();
    void sirilStarted();
    void sirilConnected();
    void sirilReady();
    void sirilCdSuccess();
    void sirilLsStarted();
    void sirilStackReady();
    void sirilMessage(QString message);

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
    QString sirilPath = "";
    QString workingDir = "";
    QString darkPath = "";
    QString flatPath = "";
    QString registrationMode = "";
    QFile* messagePipe;
    QString newImagePath = "";
};

#endif // SIRILINTERFACE_H

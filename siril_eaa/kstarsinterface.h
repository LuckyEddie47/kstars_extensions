// Provides the interface to KStars for this plugin

#ifndef KSTARSINTERFACE_H
#define KSTARSINTERFACE_H

#include <QObject>
#include <QtDBus>

class kstarsinterface : public QObject
{
    Q_OBJECT
public:
    explicit kstarsinterface(QObject *parent = nullptr);

    void dbusAccessing();
    void kstarsAccessing();
    void schedulerChecking();
    void captureChecking();
    void captureCheckingNoJobs();
    void captureGettingFileFormat();
    void captureGettingFilePath();
    void captureSetttingDisplayExternal();
    void captureJobRunning();
    void sendStacktoEkos();
    void captureStopAndReset();
    bool disconnectInterface();
    void setFITSfromFile(bool previewFromFile);
    void openFITSfile(const QString &filePath);

private slots:
    void receiverStatusChanged(bool status);
    void handleCapturedImage(QDBusMessage message);

signals:
    void dbusAccessible();
    void kstarsAccessible();
    void schedulerIdle();
    void captureIdle();
    void captureNoJobs();
    void captureFormatOkay();
    void readCaptureFilePath();
    void captureFilePath(QString filePath);
    void captureDisplaySet();
    void newCaptureImage(const QString &filePath);
    void captureImageTaken();
    void readyForNext();
    void errorMessage(QString errorDetail);
    void stopSession();

private:
    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusInterface *monInterface;

    QString cameraName = "";
    QString cameraInterface = "";
    int* capJobCount = 0;
    QString jobPath = "";

signals:
};

#endif // KSTARSINTERFACE_H

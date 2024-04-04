// Provides the interface to KStars for this plugin

#ifndef KSTARSINTERFACE_H
#define KSTARSINTERFACE_H

#include "ekosStatus.h"

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
    void stopKStars();
    void restartKStars();
    bool disconnectInterface();

public slots:

signals:
    void dbusAccessible();
    void kstarsAccessible();
    void schedulerIdle();
    void captureIdle();
    void errorMessage(QString errorDetail);
    void stopSession();
    void stoppedKS();

private:
    QDBusConnection bus = QDBusConnection::sessionBus();

    QString cameraName = "";
    QString cameraInterface = "";
    int* capJobCount = 0;
    bool haveShutdownKStars = false;

signals:
};

#endif // KSTARSINTERFACE_H

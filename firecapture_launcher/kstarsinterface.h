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
    bool checkDBus();
    bool checkKStarsService();
    bool disconnectInterface();
    CaptureState checkCaptureStatus();
    SchedulerState checkSchedulerStatus();
    bool disconnectCamera();
    void reconnectCamera();

public slots:
    void receiverStatusChanged(pluginState status);

signals:
    void stopSession();

private:
    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusInterface *monInterface;
    void getCamera();

    QString cameraName = "";
    QString cameraInterface = "";

signals:
};

#endif // KSTARSINTERFACE_H

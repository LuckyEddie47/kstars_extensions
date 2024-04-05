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
    void checkDBus();
    void checkKStars();
    void checkScheduler();
    void checkCapture();
    void stopKStars();
    void restartKStars();

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
    bool haveShutdownKStars = false;
};

#endif // KSTARSINTERFACE_H

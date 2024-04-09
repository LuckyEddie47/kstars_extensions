/* This is an example DBus interaction class for KStar_extensions
 * It provides the minimal functionality of:
 * Checking that the KStars DBus interface is present, and accessible
 * Listening for the StopExtension signal
 *
 * For a real extension the class will need to be extended
 */

#ifndef KSTARSINTERFACE_H
#define KSTARSINTERFACE_H

#include "ekosStatus.h"
#include <QtDBus>

class kstarsinterface : public QObject
{
    Q_OBJECT
public:
    explicit kstarsinterface(QObject *parent = nullptr);

public slots:
    bool kstarsStateIsValid();
    void receiverStatusChanged(bool status);

signals:
    void errorMessage(const QString errorDetail);
    void exitRequested();

private:
    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusInterface *monInterface;
//    void receiverStatusChanged(bool status);
//    void receiverStatusChanged();
};

#endif // KSTARSINTERFACE_H

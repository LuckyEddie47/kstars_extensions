/* This is an example DBus interaction class for KStar_extensions
 * It provides the minimal functionality of:
 * Checking that the KStars DBus interface is present, and accessible
 * Listening for the StopExtension signal
 *
 * For a real extension the class will need to be extended
 */

#ifndef KSTARSINTERFACE_H
#define KSTARSINTERFACE_H

#include <QtDBus>

class kstarsinterface : public QObject
{
    Q_OBJECT
public:
    explicit kstarsinterface(QObject *parent = nullptr);

public slots:
    bool kstarsStateIsValid();

signals:
    void errorMessage(const QString errorDetail);
    void exitRequested();

private slots:
    void receiverStatusChanged(QDBusMessage message);

private:
    QDBusConnection bus = QDBusConnection::sessionBus();
};

#endif // KSTARSINTERFACE_H

// Provides some common DBus strings for KStars

#ifndef KSTARSDBUSCONF_H
#define KSTARSDBUSCONF_H

#include <QString>

QString serviceName = "org.kde.kstars";
QString ksInterface = "org.kde.kstars.KStars";
QString EkosInterface = "org.kde.kstars.Ekos";
QString pathCapture = "/KStars/Ekos/Capture";
QString pathEkos = "/KStars/Ekos";
QString pathScheduler = "/KStars/Ekos/Scheduler";
QString pathINDIgeneric = "/KStars/INDI/GenericDevice";
QString introspectable = "org.freedesktop.DBus.Introspectable";
QString INDIgeneric = "org.kde.kstars.INDI.GenericDevice";

#endif // KSTARSDBUSCONF_H

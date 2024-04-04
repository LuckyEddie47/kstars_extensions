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
QString pathKStars = "/KStars";
QString ksmwInterface = "org.kde.KMainWindow";
QString showEkPath = "/kstars/MainWindow_1/actions/show_ekos";
QString qActionInterface = "org.qtproject.Qt.QAction";

#endif // KSTARSDBUSCONF_H

// Provides an exit call that can handled by the event loop

#ifndef BOMBOUT_H
#define BOMBOUT_H

#include "qcoreapplication.h"
#include <QObject>

class bombout : public QObject
{
    Q_OBJECT
public:
    explicit bombout(QObject *parent = nullptr) {
        qDebug("Exiting extension");
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
    };

signals:
};

#endif // BOMBOUT_H

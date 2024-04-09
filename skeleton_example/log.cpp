#include "log.h"
#include <QDebug>

logger::logger(QObject *parent)
    : QObject{parent}
{}

void logger::out(const QString &output)
{
    qDebug().noquote() << output;
}

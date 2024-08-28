#include "log.h"
#include <QDebug>

logger::logger(QObject *parent)
    : QObject{parent}
{}

void logger::out(QString output)
{
    qDebug().noquote() << output;
}

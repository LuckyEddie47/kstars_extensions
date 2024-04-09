#include "process.h"


process::process(QObject *parent)
    : QObject{parent}
{}

void process::startProcess()
{
    m_timer = new QTimer;
    m_timer->setInterval(2000);
    connect(m_timer, &QTimer::timeout, this, [this] {
        emit message("I'm alive");
    });
    m_timer->start();
}

void process::stopProcess()
{
    m_timer->stop();
    emit message("I'm dying");
}

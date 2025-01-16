#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "log.h"
#include "kstarsinterface.h"
#include "sirilinterface.h"
#include "confchecker.h"

#include <QObject>
#include <QStateMachine>

class statemachine : public QObject
{
    Q_OBJECT
public:
    explicit statemachine(QString appFilePath, QObject *parent = nullptr);

private slots:

signals:
    void haltMachine();

private:
    QStateMachine* machine;
    confChecker* m_confChecker;
    logger* m_logger;
    kstarsinterface* m_kstarsinterface;
    sirilinterface* m_sirilinterface;

    QString sirilPath = "";
    bool sirilRunning = false;

    void createMachine();
    void handleError(QString errorMessage);
};

#endif // STATEMACHINE_H

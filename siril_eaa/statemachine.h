#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "log.h"
#include "kstarsinterface.h"
#include "process.h"
#include "confchecker.h"

#include <QObject>
#include <QStateMachine>

class statemachine : public QObject
{
    Q_OBJECT
public:
    explicit statemachine(QString appFilePath, QObject *parent = nullptr);

private slots:
//    void checkConf();
//    void checkVersion();
//    void checkPath();

signals:
    void confExists();
    void versionValid();
    void pathValid();
//    void confValid();

private:
    QStateMachine* machine;
    confChecker* m_confChecker;
    logger* m_logger;
    kstarsinterface* m_kstarsinterface;
    process* m_process;

    void createMachine();
};

#endif // STATEMACHINE_H

#include "statemachine.h"

#include "statemachine.h"
#include "bombout.h"

#include <QFinalState>

statemachine::statemachine(QString appFilePath, QObject *parent)
    : QObject{parent}
{
    machine = new QStateMachine(this);
    m_confChecker = new confChecker(appFilePath, this);
    m_logger = new logger(this);
    m_kstarsinterface = new kstarsinterface(this);
    m_process = new process(this);

    createMachine();
}

void statemachine::createMachine()
{
    // Define states
    QState *checkingConf = new QState();
    QState *doesConfExist = new QState(checkingConf);
    QState *isConfAccessible = new QState(checkingConf);
    QState *isVersionValid = new QState(checkingConf);
    QState *isPathValid = new QState(checkingConf);
    QFinalState *confIsValid = new QFinalState(checkingConf);
    checkingConf->setInitialState(doesConfExist);

    QState *checkingEkos = new QState();
    QState *dbusAccessible = new QState(checkingEkos);
    QState *kstarsAccessible = new QState(checkingEkos);
    QState *schedulerIdle = new QState(checkingEkos);
    QState *captureIdle = new QState(checkingEkos);
    QState *captureJobValid = new QState(checkingEkos);
    QFinalState *ekosValid = new QFinalState(checkingEkos);
    checkingEkos->setInitialState(dbusAccessible);

    QState *settingUpSiril = new QState();
    QState *launchingSiril = new QState(settingUpSiril);
    QState *checkingSiril = new QState(settingUpSiril);
    QState *settingWDSiril = new QState(settingUpSiril);
    QState *settingLSSiril = new QState(settingUpSiril);
    QFinalState *sirilSetup = new QFinalState(settingUpSiril);
    settingUpSiril->setInitialState(launchingSiril);

    QState *runningLS = new QState();
    QState *settingEkosJob = new QState(runningLS);
    QState *runningEkosJob = new QState(runningLS);
    QState *receivingFrame = new QState(runningLS);
    QState *stackingFrame = new QState(runningLS);
    QState *receivingStack = new QState(runningLS);
    QFinalState *livestackingEnd = new QFinalState(runningLS);
    runningLS->setInitialState(settingEkosJob);

    QState *stopping = new QState();
    QState *stoppingEkosJob = new QState(stopping);
    QState *resettingEkos = new QState(stopping);
    QState *closingSiril = new QState(stopping);
    QFinalState *stopped = new QFinalState(stopping);
    stopping->setInitialState(stoppingEkosJob);

    QState *error = new QState();

    QFinalState *finish = new QFinalState();

    // Assemble machine
    machine->addState(checkingConf);
    machine->addState(checkingEkos);
    machine->addState(settingUpSiril);
    machine->addState(runningLS);
    machine->addState(stopping);
    machine->addState(stopped);
    machine->addState(error);
    machine->setInitialState(checkingConf);

    // Define state transistions
    connect(doesConfExist, &QAbstractState::entered, m_confChecker, &confChecker::confExisting);
    doesConfExist->addTransition(m_confChecker, SIGNAL(confExists), isConfAccessible);
    connect(isConfAccessible, &QAbstractState::entered, m_confChecker, &confChecker::confAccessing);
    isConfAccessible->addTransition(m_confChecker, SIGNAL(confAccessible), isVersionValid)    ;
    connect(isVersionValid, &QAbstractState::entered, m_confChecker, &confChecker::versionValidating);
    isVersionValid->addTransition(m_confChecker, SIGNAL(versionValid), isPathValid);
    connect(isPathValid, &QAbstractState::entered, m_confChecker, &confChecker::pathValidating);
    isPathValid->addTransition(m_confChecker, SIGNAL(pathValid), confIsValid);
    checkingConf->addTransition(checkingConf, SIGNAL(finished), checkingEkos);

    // State function overrides

}

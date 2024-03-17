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
    m_sirilinterface = new sirilinterface(this);

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
    QState *checkingDbus = new QState(checkingEkos);
    QState *checkingKstars = new QState(checkingEkos);
    QState *checkingScheduler = new QState(checkingEkos);
    QState *checkingCapture = new QState(checkingEkos);
    QState *checkingCaptureNoJobs = new QState(checkingEkos);
    QState *gettingCaptureFileFormat = new QState(checkingEkos);
    QState *gettingCaptureFilePath = new QState(checkingEkos);
    QFinalState *ekosIsValid = new QFinalState(checkingEkos);
    checkingEkos->setInitialState(checkingDbus);

    QState *settingUpSiril = new QState();
    QState *launchingSiril = new QState(settingUpSiril);
    QState *connectingSiril = new QState(settingUpSiril);
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
    // Configuration file
    connect(doesConfExist, &QAbstractState::entered, m_confChecker, &confChecker::confExisting);
    doesConfExist->addTransition(m_confChecker, SIGNAL(confExists()), isConfAccessible);
    connect(isConfAccessible, &QAbstractState::entered, m_confChecker, &confChecker::confAccessing);
    isConfAccessible->addTransition(m_confChecker, SIGNAL(confAccessible()), isVersionValid)    ;
    connect(isVersionValid, &QAbstractState::entered, m_confChecker, &confChecker::versionValidating);
    isVersionValid->addTransition(m_confChecker, SIGNAL(versionValid()), isPathValid);
    connect(isPathValid, &QAbstractState::entered, m_confChecker, &confChecker::pathValidating);
    isPathValid->addTransition(m_confChecker, SIGNAL(pathValid()), confIsValid);
    connect(m_confChecker, &confChecker::sirilPathIs, m_sirilinterface, &sirilinterface::setSirilPath); // Note odd one out passing path
    checkingConf->addTransition(checkingConf, SIGNAL(finished()), checkingEkos);

    // KStars
    connect(checkingDbus, &QAbstractState::entered, m_kstarsinterface, &kstarsinterface::dbusAccessing);
    checkingDbus->addTransition(m_kstarsinterface, SIGNAL(dbusAccessible()), checkingKstars);
    connect(checkingKstars, &QAbstractState::entered, m_kstarsinterface, &kstarsinterface::kstarsAccessing);
    checkingKstars->addTransition(m_kstarsinterface, SIGNAL(kstarsAccessible()), checkingScheduler);
    connect(checkingScheduler, &QAbstractState::entered, m_kstarsinterface, &kstarsinterface::schedulerChecking);
    checkingScheduler->addTransition(m_kstarsinterface, SIGNAL(schedulerIdle()), checkingCapture);
    connect(checkingCapture, &QAbstractState::entered, m_kstarsinterface, &kstarsinterface::captureChecking);
    checkingCapture->addTransition(m_kstarsinterface, SIGNAL(captureIdle()), checkingCaptureNoJobs);
    connect(checkingCaptureNoJobs, &QAbstractState::entered, m_kstarsinterface, &kstarsinterface::captureCheckingNoJobs);
    checkingCaptureNoJobs->addTransition(m_kstarsinterface, SIGNAL(captureNoJobs()), gettingCaptureFileFormat);
    connect(gettingCaptureFileFormat, &QAbstractState::entered, m_kstarsinterface, &kstarsinterface::captureGettingFileFormat);
    gettingCaptureFileFormat->addTransition(m_kstarsinterface, SIGNAL(captureFormatOkay()), gettingCaptureFilePath);
    connect(gettingCaptureFilePath, &QAbstractState::entered, m_kstarsinterface, &kstarsinterface::captureGettingFilePath);
    gettingCaptureFilePath->addTransition(m_kstarsinterface, SIGNAL(readCaptureFilePath()), settingUpSiril);
    connect(m_kstarsinterface, &kstarsinterface::captureFilePath, m_sirilinterface, &sirilinterface::setWD); // Note odd one out passing path

    // Siril
    connect(launchingSiril, &QAbstractState::entered, m_sirilinterface, &sirilinterface::startSiril);
    launchingSiril->addTransition(m_sirilinterface, SIGNAL(sirilStarted()), connectingSiril);
    connect(connectingSiril, &QAbstractState::entered, m_sirilinterface, &sirilinterface::connectSiril);
    connectingSiril->addTransition(m_sirilinterface, SIGNAL(sirilReady()), settingWDSiril);
    connect(settingWDSiril, &QAbstractState::entered, m_sirilinterface, &sirilinterface::setSirilWD);
    settingWDSiril->addTransition(m_sirilinterface, SIGNAL(sirilCdSuccess()), settingLSSiril);
    connect(settingLSSiril, & QAbstractState::entered, m_sirilinterface, &sirilinterface::setSirilLS);
    settingLSSiril->addTransition(m_sirilinterface, SIGNAL(sirilLsStarted()), runningLS);

    // Connect error signals
    connect(m_confChecker, &confChecker::errorMessage, this, &statemachine::handleError);
    connect(m_kstarsinterface, &kstarsinterface::errorMessage, this, &statemachine::handleError);
    connect(m_sirilinterface, &sirilinterface::errorMessage, this, &statemachine::handleError);

    machine->start();
}

void statemachine::handleError(QString errorMessage)
{
    m_logger->out(errorMessage);
    m_logger->out("Closing extension");
    m_kstarsinterface->captureStopAndReset();
    m_sirilinterface->sendSirilCommand("exit");
}

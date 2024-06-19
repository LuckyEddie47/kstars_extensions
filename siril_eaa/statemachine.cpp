#include "statemachine.h"
#include "bombout.h"

#include <QFinalState>
#include <QTimer>

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
    QState *isDarkPassed = new QState(checkingConf);
    QState *isFlatPassed = new QState(checkingConf);
    QState *isRegistationPassed = new QState(checkingConf);
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
    checkingEkos->setInitialState(checkingDbus);

    QState *settingUpSiril = new QState();
    QState *launchingSiril = new QState(settingUpSiril);
    QState *connectingSiril = new QState(settingUpSiril);
    QState *settingWDSiril = new QState(settingUpSiril);
    QState *settingLSSiril = new QState(settingUpSiril);
    settingUpSiril->setInitialState(launchingSiril);

    QState *runningLS = new QState();
    QState *settingEkosJob = new QState(runningLS);
    QState *runningEkosJob = new QState(runningLS);
    QState *stackingFrame = new QState(runningLS);
    QState *receivingStack = new QState(runningLS);
    runningLS->setInitialState(settingEkosJob);

    QFinalState *finish = new QFinalState();

    // Assemble machine
    machine->addState(checkingConf);
    machine->addState(checkingEkos);
    machine->addState(settingUpSiril);
    machine->addState(runningLS);
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
    connect(m_confChecker, &confChecker::sirilPathIs, m_sirilinterface, &sirilinterface::setSirilPath); // Note odd one out passing Siril path
    isPathValid->addTransition(m_confChecker, SIGNAL(pathValid()), isDarkPassed);
    connect(isDarkPassed, &QAbstractState::entered, m_confChecker, &confChecker::darkChecking);
    connect(m_confChecker, &confChecker::darkPathIs, m_sirilinterface, &sirilinterface::setDarkPath); // Note odd one out passing dark path
    isDarkPassed->addTransition(m_confChecker, SIGNAL(darkChecked()), isFlatPassed);
    connect(isFlatPassed, &QAbstractState::entered, m_confChecker, &confChecker::flatChecking);
    connect(m_confChecker, &confChecker::flatPathIs, m_sirilinterface, &sirilinterface::setFlatPath); // Note odd one out passing flat path
    isFlatPassed->addTransition(m_confChecker, SIGNAL(flatChecked()), isRegistationPassed);
    connect(isRegistationPassed, &QAbstractState::entered, m_confChecker, &confChecker::registrationChecking);
    connect(m_confChecker, &confChecker::registrationIs, m_sirilinterface, &sirilinterface::setRegistrationMode);  // Note odd one out passing registration mode
    isRegistationPassed->addTransition(m_confChecker, SIGNAL(registrationChecked()), confIsValid);
    checkingConf->addTransition(checkingConf, SIGNAL(finished()), checkingEkos);

    // KStars check and configure
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
    connect(m_kstarsinterface, &kstarsinterface::captureFilePath, m_sirilinterface, &sirilinterface::setWD); // Note odd one out passing working dir

    // Siril launch and configure
    connect(launchingSiril, &QAbstractState::entered, m_sirilinterface, &sirilinterface::startSiril);
    launchingSiril->addTransition(m_sirilinterface, SIGNAL(sirilStarted()), connectingSiril);
    connect(connectingSiril, &QAbstractState::entered, m_sirilinterface, &sirilinterface::connectSiril);
    connectingSiril->addTransition(m_sirilinterface, SIGNAL(sirilReady()), settingWDSiril);
    connect(settingWDSiril, &QAbstractState::entered, m_sirilinterface, &sirilinterface::setSirilWD);
    settingWDSiril->addTransition(m_sirilinterface, SIGNAL(sirilCdSuccess()), settingLSSiril);
    connect(settingLSSiril, & QAbstractState::entered, m_sirilinterface, &sirilinterface::setSirilLS);
    settingLSSiril->addTransition(m_sirilinterface, SIGNAL(sirilLsStarted()), runningLS);

    connect(settingEkosJob, &QAbstractState::entered, m_kstarsinterface, &kstarsinterface::captureSetttingDisplayExternal);
    settingEkosJob->addTransition(m_kstarsinterface, SIGNAL(captureDisplaySet()), runningEkosJob);
    connect(runningEkosJob, &QAbstractState::entered, m_kstarsinterface, &kstarsinterface::captureJobRunning);
    runningLS->addTransition(m_kstarsinterface, SIGNAL(captureImageTaken()), stackingFrame);
    connect(m_kstarsinterface, &kstarsinterface::newCaptureImage, m_sirilinterface, &sirilinterface::newImageFromKStars); // Note odd one out passing filePath
    connect(stackingFrame, &QAbstractState::entered, m_sirilinterface, &sirilinterface::sendImageToSiril);
    stackingFrame->addTransition(m_sirilinterface, SIGNAL(sirilStackReady()), receivingStack);
    connect(receivingStack, &QAbstractState::entered, m_kstarsinterface, &kstarsinterface::sendStacktoEkos);
    receivingStack->addTransition(m_kstarsinterface, SIGNAL(readyForNext()), runningEkosJob);

    // Connect error signals
    connect(m_confChecker, &confChecker::errorMessage, this, &statemachine::handleError);
    connect(m_kstarsinterface, &kstarsinterface::errorMessage, this, &statemachine::handleError);
    connect(m_sirilinterface, &sirilinterface::errorMessage, this, &statemachine::handleError);

    connect(m_kstarsinterface, &kstarsinterface::stopSession, this, [this] {
        handleError(QString(tr("Exit requested")));
    });

    // Siril message passing
    connect(m_sirilinterface, &sirilinterface::sirilMessage, m_logger, &logger::out);

    machine->start();
}

// Error passing and shutdown
void statemachine::handleError(QString errorMessage)
{
    m_logger->out(errorMessage);
    m_kstarsinterface->captureStopAndReset();
    m_sirilinterface->sendSirilCommand("exit");

    // Remove Siril temporary files
    QDir m_dir(m_sirilinterface->getWD());
    m_dir.setNameFilters(QStringList("live_stack_*.fit"));
    QFileInfoList entries = m_dir.entryInfoList();
    foreach (QFileInfo entry, entries) {
        if (entry.isSymbolicLink()) {
            QFile::remove(entry.absoluteFilePath());
        }
    }
    QFile::remove(QString(m_sirilinterface->getWD()).append("%1%2").arg("/", "live_stack_.seq"));

    /* We give Siril a little time to exit gracefully and then
     * close the extension. Unable to monitor for Siril closure
     * as the message pipe is closed as part of its exit
     */
    QTimer::singleShot(500, this, [this] {
        m_sirilinterface->stopProgram();
        bombout();
    });
}

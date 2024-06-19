#include "confchecker.h"
#include "kstarsVersion.h"

confChecker::confChecker(QString appFilePath, QObject *parent)
    : QObject{parent}
{
    confFilePath = appFilePath.append(".conf");
}
 // Check that the configuration file exists
void confChecker::confExisting()
{
    confFile.setFileName(confFilePath);
    if (confFile.exists()) {
        emit confExists();
    } else {
        emit errorMessage(tr("Configuration file %1 does not exist").arg(confFilePath));
    }
}

// Check that the configuration file can be accessed
void confChecker::confAccessing()
{
    if (confFile.open(QIODevice::ReadOnly) && confFile.isReadable()) {
        emit confAccessible();
    } else {
        emit errorMessage(tr("Configuration file %1 is not accessible").arg(confFilePath));
    }
}

// Check that the configuration file has a valid KStars version
void confChecker::versionValidating()
{
    bool okayToProceed = false;
    confTS.setDevice(&confFile);
    while (!confTS.atEnd()) {
        QString confLine = confTS.readLine();
        if (confLine.contains("minimum_kstars_version=")) {
            QString minVersion = confLine.right(confLine.length() - (confLine.indexOf("=")) - 1);
            QStringList minVersionElements = minVersion.split(".");
            if (minVersionElements.count() == 3) {
                QList <int> minVersionElementInts;
                foreach (QString element, minVersionElements) {
                    if (element.toInt() || element == "0") {
                        minVersionElementInts.append(element.toInt());
                    } else break;
                }
                if (minVersionElementInts.count() == minVersionElements.count()) {
                    QStringList KStarsVersionElements = QString(MIN_KSTARS_VERSION).split(".");
                    QList <int> KStarsVersionElementInts;
                    foreach (QString element, KStarsVersionElements) {
                        if (element.toInt() || element == "0") {
                            KStarsVersionElementInts.append(element.toInt());
                        }
                    }
                    if (KStarsVersionElementInts.at(0) > minVersionElementInts.at(0)) {
                        okayToProceed = true;
                    } else if ((KStarsVersionElementInts.at(0) == minVersionElementInts.at(0)) &&
                               (KStarsVersionElementInts.at(1) > minVersionElementInts.at(1))) {
                        okayToProceed = true;
                    } else if ((KStarsVersionElementInts.at(0) == minVersionElementInts.at(0)) &&
                               (KStarsVersionElementInts.at(1) == minVersionElementInts.at(1)) &&
                               (KStarsVersionElementInts.at(2) >= minVersionElementInts.at(2))) {
                        okayToProceed = true;
                    }
                }
            }
        }
    }
    if (okayToProceed) {
        emit versionValid();
    } else {
        emit errorMessage(tr("KStars version incompatible"));
    }
}

// Check that the configuration file has a valid Siril path
void confChecker::pathValidating()
{
    bool okayToProceed = false;
    confTS.seek(0);
    while (!confTS.atEnd()) {
        QString confLine = confTS.readLine();
        if (confLine.contains("siril_path=")) {
            sirilPath = confLine.right(confLine.length() - (confLine.indexOf("=")) - 1);
            QFile Siril;
            Siril.setFileName(sirilPath);
            if (Siril.exists()) {
                okayToProceed = true;
            }
        }
    }
    if (okayToProceed) {
        emit pathValid();
        emit sirilPathIs(sirilPath);
    } else {
        emit errorMessage(tr("siril_path %1 in configuration file does not exist").arg(sirilPath));
    }
}

// Check if the configuration file has a dark path
void confChecker::darkChecking()
{
    QString darkPath = "";
    confTS.seek(0);
    while (!confTS.atEnd()) {
        QString confLine = confTS.readLine();
        if (confLine.contains("dark_path=")) {
            QString testPath = confLine.right(confLine.length() - (confLine.indexOf("=")) - 1);
            QFile dark;
            dark.setFileName(testPath);
            if (dark.exists()) {
                darkPath = testPath;
            }
        }
    }
        emit darkPathIs(darkPath);
        emit darkChecked();
}

// Check if the configuration file has a flat path
void confChecker::flatChecking()
{
    QString flatPath = "";
    confTS.seek(0);
    while (!confTS.atEnd()) {
        QString confLine = confTS.readLine();
        if (confLine.contains("flat_path=")) {
            QString testPath = confLine.right(confLine.length() - (confLine.indexOf("=")) - 1);
            QFile flat;
            flat.setFileName(testPath);
            if (flat.exists()) {
                flatPath = testPath;
            }
        }
    }
    emit flatPathIs(flatPath);
    emit flatChecked();
}

// Check if the configuration file has a registration option
void confChecker::registrationChecking()
{
    QString registrationMode = "";
    confTS.seek(0);
    while (!confTS.atEnd()) {
        QString confLine = confTS.readLine();
        if (confLine.contains("registration_mode=")) {
            registrationMode = confLine.right(confLine.length() - (confLine.indexOf("=")) - 1);
        }
    }
    emit registrationIs(registrationMode);
    emit registrationChecked();
}

#include "confchecker.h"
#include "kstarsVersion.h"

confChecker::confChecker(QString appFilePath, QObject *parent)
    : QObject{parent}
{
    confFilePath = appFilePath.append(".conf");
}

void confChecker::confExisting()
{
    confFile.setFileName(confFilePath);
    if (confFile.exists()) {
        emit confExists();
    } else {
        emit errorMessage(QString("Configuration file %1 does not exist").arg(confFilePath));
    }
}

void confChecker::confAccessing()
{
    if (confFile.open(QIODevice::ReadOnly) && confFile.isReadable()) {
        emit confAccessible();
    } else {
        emit errorMessage(QString("Configuration file %1 is not accessible").arg(confFilePath));
    }
}

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
                    if ((minVersionElementInts.at(0) == KStarsVersionElementInts.at(0)) &&
                        (minVersionElementInts.at(1) == KStarsVersionElementInts.at(1)) &&
                        (minVersionElementInts.at(2) == KStarsVersionElementInts.at(2))){
                        okayToProceed = true;
                    }
                }
            }
        }
    }
    if (okayToProceed) {
        emit versionValid();
    } else {
        emit errorMessage("KStars version incompatible");
    }
}

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
        emit errorMessage(QString("siril_path %1 in configuration file does not exist").arg(sirilPath));
    }
}

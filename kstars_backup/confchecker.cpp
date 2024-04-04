#include "confchecker.h"
#include "kstarsVersion.h"

confChecker::confChecker(QString appFilePath, QObject *parent)
    : QObject{parent}
{
    confFilePath = appFilePath.append(".conf");
}
 // Check that the configuration file exists
void confChecker::start()
{
    confFile.setFileName(confFilePath);
    if (confFile.exists()) {
        confAccessing();
    } else {
        emit errorMessage(QString("Configuration file %1 does not exist").arg(confFilePath));
    }
}

// Check that the configuration file can be accessed
void confChecker::confAccessing()
{
    if (confFile.open(QIODevice::ReadOnly) && confFile.isReadable()) {
        versionValidating();
    } else {
        emit errorMessage(QString("Configuration file %1 is not accessible").arg(confFilePath));
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
        pathValidating();
    } else {
        emit errorMessage("KStars version incompatible");
    }
}

// Read any paths from the configuration file
void confChecker::pathValidating()
{
    QString home = "";
    if (confFilePath.startsWith("/home/")) {
        home = confFilePath;
        while (home.count("/") > 2) {
            home = home.left(home.lastIndexOf("/"));
        }
    }
    confTS.seek(0);
    while (!confTS.atEnd()) {
        QString confLine = confTS.readLine();
        if (confLine.contains("path=")) {
            confLine = confLine.right(confLine.length() - (confLine.indexOf("=")) - 1);
            if (confLine.startsWith("~")) {
                confLine.replace("~", home);
            }
            paths << confLine;
        }
    }
        emit pathsFound(paths);
}

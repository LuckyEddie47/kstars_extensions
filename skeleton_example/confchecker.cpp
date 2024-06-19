#include "confchecker.h"

#include <QFile>
#include <QTextStream>

confChecker::confChecker(const QString &appFilePath, QObject *parent)
    : QObject{parent}
{
    confFilePath = appFilePath;
    confFilePath.append(".conf");
}

// Validate the configuration file
bool confChecker::isValid()
{
    bool confIsValid = false;

    QFile confFile(confFilePath);
    if (confFile.exists()) {
        if (confFile.open(QIODevice::ReadOnly) && confFile.isReadable()) {
            QTextStream confTS = QTextStream(&confFile);
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
                                confIsValid = true;
                            } else if ((KStarsVersionElementInts.at(0) == minVersionElementInts.at(0)) &&
                                       (KStarsVersionElementInts.at(1) > minVersionElementInts.at(1))) {
                                confIsValid = true;
                            } else if ((KStarsVersionElementInts.at(0) == minVersionElementInts.at(0)) &&
                                       (KStarsVersionElementInts.at(1) == minVersionElementInts.at(1)) &&
                                       (KStarsVersionElementInts.at(2) >= minVersionElementInts.at(2))) {
                                confIsValid = true;
                            } else {
                                emit errorMessage(QString("Incorrect minimum_kstars_version in %1").arg(confFilePath));
                            }
                        } else {
                            emit errorMessage(QString("Malformed minimum_kstars_version in %1").arg(confFilePath));
                        }
                    } else {
                        emit errorMessage(QString("Malformed minimum_kstars_version in %1").arg(confFilePath));
                    }
                }
            }
            if (!confIsValid) {
                emit errorMessage(QString("No minimum_kstars_version in %1").arg(confFilePath));
            }
        } else {
            emit errorMessage(QString("Can't access %1").arg(confFilePath));
        }
    }  else {
        emit errorMessage(QString("%1 does not exist").arg(confFilePath));
    }

    return confIsValid;
}

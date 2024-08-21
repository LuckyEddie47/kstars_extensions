/* This is an example configuration file checker class for KStars_extensions
 * It provides the minimum checks of:
 * Does the configuration file exist, is correctly named, in the same directory as the
 * executable, and is the configuration file accessible?
 * Does the configuration file contain the required minimum_kstars_version=n.n.n and does
 * that n.n.n match the MIN_KSTARS_VERSION defined below?
 *
 * For a real extension the class will need to be extended
 */

#ifndef CONFCHECKER_H
#define CONFCHECKER_H

#include <QObject>

// This is the earlies version of KStars that this plugin targets
#define MIN_KSTARS_VERSION "3.7.3"

class confChecker : public QObject
{
    Q_OBJECT
public:
    explicit confChecker(const QString &appFilePath, QObject *parent = nullptr);

public slots:
    bool isValid();

signals:
    void errorMessage(const QString errorDetail);

private:
    QString confFilePath = "";
};

#endif // CONFCHECKER_H

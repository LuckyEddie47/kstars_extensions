#ifndef CONFCHECKER_H
#define CONFCHECKER_H

#include <QObject>
#include <QFile>
#include <QTextStream>

class confChecker : public QObject
{
    Q_OBJECT
public:
    explicit confChecker(QString appFilePath, QObject *parent = nullptr);

public slots:
    void start();
    void confAccessing();
    void versionValidating();
    void pathValidating();

signals:
    void pathsFound(const QStringList &paths);

    void errorMessage(const QString errorDetail);

private:
    QString confFilePath = "";
    QFile confFile;
    QTextStream confTS;
    QStringList paths;
};

#endif // CONFCHECKER_H

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
    void confExisting();
    void confAccessing();
    void versionValidating();
    void pathValidating();

signals:
    void confExists();
    void confAccessible();
    void versionValid();
    void pathValid();
    void errorMessage(QString errorDetail);

private:
    QString confFilePath = "";
    QString SirilPath = "";
    QFile confFile;
    QTextStream confTS;
};

#endif // CONFCHECKER_H

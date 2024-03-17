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
    void darkChecking();
    void flatChecking();
    void registrationChecking();

signals:
    void confExists();
    void confAccessible();
    void versionValid();
    void pathValid();
    void sirilPathIs(QString path);
    void darkChecked();
    void darkPathIs(QString path);
    void flatChecked();
    void flatPathIs(QString path);
    void registrationChecked();
    void registrationIs(QString registration);

    void errorMessage(QString errorDetail);

private:
    QString confFilePath = "";
    QString sirilPath = "";
    QFile confFile;
    QTextStream confTS;
};

#endif // CONFCHECKER_H

#ifndef PROCESS_H
#define PROCESS_H

#include <QObject>
#include <QProcess>

class process : public QObject
{
    Q_OBJECT
public:
    explicit process(QObject *parent = nullptr);
    void startProgram(QString path);

signals:
    void programFinished();
    void programStarted();

private:
    QProcess programProcess;
};

#endif // PROCESS_H

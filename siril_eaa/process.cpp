#include "process.h"

process::process(QObject *parent)
    : QObject{parent}
{}

void process::startProgram(QString path)
{
    bool result = false;

    QString wd = path.left(path.lastIndexOf("/"));
    QStringList arguments;
    arguments << "--no-confirm";

    QObject::connect(&programProcess, &QProcess::started, this, [=] (){
        emit programStarted();
    });

    QObject::connect(&programProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this,
                     [=](int exitCode, QProcess::ExitStatus exitStatus)
                     {
                         emit programFinished();
                     });

    programProcess.setWorkingDirectory(wd);
    programProcess.start(path, arguments);
}

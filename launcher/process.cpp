#include "process.h"

#include <QFile>
#include <QTextStream>

process::process(QObject *parent)
    : QObject{parent}
{}

// Launch program
void process::startProgram(const QString &path)
{
    bool result = false;

    QString wd = path.left(path.lastIndexOf("/"));
    QStringList arguments;

    QObject::connect(&programProcess, &QProcess::started, this, [=] (){
        emit programStarted();
    });

    QObject::connect(&programProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this,
                     [=](int exitCode, QProcess::ExitStatus exitStatus)
                     {
                         emit programFinished();
                     });

    programProcess.setWorkingDirectory(wd);
    programProcess.setProcessChannelMode(QProcess::ForwardedErrorChannel);

    programProcess.start(path, arguments);
}

// Close program
void process::stopProgram()
{
    programProcess.close();
}

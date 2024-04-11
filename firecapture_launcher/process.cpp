#include "process.h"

#include <QFile>
#include <QTextStream>

process::process(QObject *parent)
    : QObject{parent}
{}

/* Check the FireCapture start script supports the --no-confirm option
 * if not back it up and then patch it
 */
bool process::startScriptUpToDate(const QString &path)
{
    bool upToDate = false;

    QStringList lines;
    QFile startScript(path);
    if (startScript.exists()) {
        if (startScript.open(QIODevice::ReadWrite) && startScript.isReadable()) {
            QTextStream scriptTS = QTextStream(&startScript);
            while (!scriptTS.atEnd()) {
                lines.append(scriptTS.readLine());
            }
            bool oldfile = false;
            for (int line = lines.count() - 1; line > 0; line --) {
                if (!upToDate && !oldfile) {
                    if (lines.at(line) == "") {
                        lines.removeAt(line);
                    } else {
                        if ((line > 2) &&
                            (lines.at(line).simplified().contains("fi")) &&
                            (lines.at(line - 1).simplified().contains("read -n 1 -s -r -p \"Press any key to continue\"")) &&
                            (lines.at(line - 2).simplified().contains("if ! { [ \"$#\" -eq 1 ] && [[ \"$1\" == \"--no-confirm\" ]]; }; then"))) {
                            startScript.close();
                            upToDate = true;
                        } else {
                            startScript.close();
                            oldfile = true;
                        }
                    }
                }
            }
            if (!upToDate) {
                // Create a backup copy of the startup script before patching
                QFile m_backup(QString(path).append(".bak"));
                if (m_backup.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    QTextStream bakTS(&m_backup);
                    for (int line = 0; line < lines.count(); line ++) {
                        bakTS << lines.at(line) << Qt::endl;
                    }
                    m_backup.close();
                };
                if (!m_backup.exists()) {
                    return upToDate;
                }
                startScript.close();

                // Write out and patch the startup script
                startScript.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
                QTextStream outTS(&startScript);
                for (int line = 0; line < lines.count() - 2; line ++) {
                    outTS << lines.at(line) << Qt::endl;
                }
                outTS << Qt::endl;
                outTS << "if ! { [ \"$#\" -eq 1 ] && [[ \"$1\" == \"--no-confirm\" ]]; }; then" << Qt::endl;
                outTS << "   " << lines.at(lines.count() - 1) << Qt::endl;
                outTS << "fi" << Qt::endl;
                startScript.close();
                upToDate = true;
            }
        }
    }

    return upToDate;
}


// Launch FireCapture
void process::startProgram(const QString &path)
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
    programProcess.setProcessChannelMode(QProcess::ForwardedErrorChannel);

    programProcess.start(path, arguments);
}

// Close FireCapture
void process::stopProgram()
{
    programProcess.terminate();
}

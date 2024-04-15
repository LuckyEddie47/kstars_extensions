#include <QCoreApplication>
#include <QObject>

#include "statemachine.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("Siril_EAA");
    QObject *parent;

    statemachine m_statemachine(QString("%1%2%3").arg(app.applicationDirPath(), "/", app.applicationName()));

    return app.exec();
}

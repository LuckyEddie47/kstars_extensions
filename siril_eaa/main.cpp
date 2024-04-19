#include <QCoreApplication>
#include <QObject>
#include <QTranslator>

#include "statemachine.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QTranslator m_translator;
    if (m_translator.load(QLocale(), ":/i18n/Siril_EAA")) {
        app.installTranslator(&m_translator);
    }
    app.setApplicationName("Siril_EAA");
    QObject *parent;

    statemachine m_statemachine(QString("%1%2%3").arg(app.applicationDirPath(), "/", app.applicationName()));

    return app.exec();
}
